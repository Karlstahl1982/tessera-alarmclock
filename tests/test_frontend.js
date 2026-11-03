// Runtime smoke-test for the dependency-free Home Assistant card, without a browser.
const fs=require('fs'),vm=require('vm'),assert=require('assert'),yamlIgnore=void 0;
const registration=new Map();
global.HTMLElement=class {attachShadow(){this.shadowRoot={innerHTML:'',querySelectorAll(){return []},querySelector(){return null}};return this.shadowRoot;}};
global.customElements={define:(n,c)=>registration.set(n,c),get:n=>registration.get(n)};
global.window={customCards:[]};
global.confirm=()=>true;
vm.runInThisContext(fs.readFileSync(require('path').join(__dirname,'../ha_card/tessera-alarmclock-card.js'),'utf8'));
const Card=registration.get('tessera-alarmclock-card');
assert(Card,'Custom card is registered');
const states={};
function add(domain,id,label,state){states[domain+'.'+id]={entity_id:domain+'.'+id,state:String(state),attributes:{friendly_name:'Andy '+label}};}
for(let i=1;i<=5;i++){
 for(const [domain,key,val] of [['time','Uhrzeit','07:00:00'],['date','Datum','2026-11-04'],['switch','aktiv','off'],['select','Modus','Next'],['select','Klang','Gentle'],['select','Schlummerminuten','5'],['number','Lautstärke','60'],['text','Wochentage','Mo,Di,Mi,Do,Fr'],['text','Name','Wecker '+i]]){
   add(domain,`ac_${key.toLowerCase()}_${i}`,`AlarmClock ${i} ${key}`,val);
 }
}
add('sensor','ac_occupied','AlarmClock Belegung','00000');
add('text','ac_action','AlarmClock Verwaltung','');
add('button','ac_test','AlarmClock Testalarm','unknown');
add('button','ac_stop','AlarmClock Alarm stoppen','unknown');
const calls=[];
const hass={states,async callService(domain,service,body){
  calls.push([domain,service,body]);
  const ent=states[body.entity_id];assert(ent,body.entity_id);
  if(domain==='switch')ent.state=service==='turn_on'?'on':'off';
  if(domain==='time'){
    assert.equal(ent.entity_id.split('.')[0],'time');
    assert.match(body.time,/^\d{2}:\d{2}:\d{2}$/);
    ent.state=body.time;
  }
  if(domain==='date'){
    assert.equal(ent.entity_id.split('.')[0],'date');
    assert.match(body.date,/^\d{4}-\d{2}-\d{2}$/);
    ent.state=body.date;
  }
  if(domain==='select')ent.state=body.option;
  if(domain==='text'){
    ent.state=body.value;
    if(ent.entity_id==='text.ac_action'){
      const [op,n]=body.value.split(':');const bits=states['sensor.ac_occupied'].state.split('');
      bits[Number(n)-1]=op==='add'?'1':'0';states['sensor.ac_occupied'].state=bits.join('');
    }
  }
  if(domain==='number')ent.state=String(body.value);
}};
(async()=>{
 const c=new Card();c.setConfig({title:'Wecker'});c.hass=hass;
 // Prevent partial saves: a mismatched domain must fail before disabling an alarm.
 const timeEntity=states['time.ac_uhrzeit_1'];
 delete states['time.ac_uhrzeit_1'];
 states['datetime.ac_uhrzeit_1']={...timeEntity,entity_id:'datetime.ac_uhrzeit_1'};
 c._slot=-1;c._mode='wizard';c._draft={...c._slotData(0),time:'06:45',name:'Test'};
 await assert.rejects(c._save(),/erwartet time, gefunden datetime/);
 assert.equal(calls.length,0,'A malformed time domain must not change ESP32 entities');
 delete states['datetime.ac_uhrzeit_1'];states['time.ac_uhrzeit_1']=timeEntity;
 assert.deepStrictEqual(c._occupied(),[false,false,false,false,false]);
 c._slot=-1;c._mode='wizard';c._draft=c._slotData(0);c._draft.name='Schule';c._draft.time='06:45';
 c._draft.mode='Weekly';c._draft.days=['Mo','Di','Mi','Do','Fr'];c._draft.enabled=true;
 await c._save();
 assert(states['sensor.ac_occupied'].state==='10000');
 assert(states['switch.ac_aktiv_1'].state==='on');
 assert(states['text.ac_name_1'].state==='Schule');
 assert(states['time.ac_uhrzeit_1'].state==='06:45:00');
 assert(states['date.ac_datum_1'].state==='2026-11-04');
 assert(calls.some(([domain,service,body])=>domain==='time'&&service==='set_value'&&body.time==='06:45:00'));
 assert(calls.some(([domain,service,body])=>domain==='date'&&service==='set_value'&&body.date==='2026-11-04'));
 assert(!calls.some(([domain])=>domain==='datetime'));
 c._slot=0;c._mode='wizard';c._draft=c._slotData(0);
 await c._remove();
 assert(states['sensor.ac_occupied'].state==='00000');
 assert(states['switch.ac_aktiv_1'].state==='off');

 // Dedicated edit screen rather than forcing an existing alarm through the creation wizard.
 const editor=new Card();editor.setConfig({title:'Wecker'});editor.hass=hass;
 editor._slot=0;editor._mode='edit';editor._draft={...editor._slotData(0),name:'Schule',mode:'Weekly'};
 const html=editor._editor();
 for(const required of ['edit-hero','edit-grid','Weckzeit','data-action="save-edit"','data-action="day"','data-action="delete"'])
   assert(html.includes(required),`Editing surface missing: ${required}`);
 await editor._action({dataset:{action:'snooze-choice',value:'10'}});
 assert.strictEqual(editor._draft.snooze,'10','Snooze selection must update draft');
 await editor._action({dataset:{action:'enabled-choice',value:'on'}});
 assert.strictEqual(editor._draft.enabled,true,'Enabled selection must update draft');
 editor._draft.time='06:35';
 const priorCount=calls.length;
 await editor._action({dataset:{action:'save-edit'}});
 assert.strictEqual(editor._mode,'list','Saving edit must return to list');
 assert.equal(states['time.ac_uhrzeit_1'].state,'06:35:00');
 assert.equal(states['select.ac_schlummerminuten_1'].state,'10');
 assert(calls.length>priorCount,'Save edit must invoke HA services');
 // Regression: slider percentage must update live without re-creating the form.
 const slider={value:'60',listeners:{},addEventListener(type,fn){this.listeners[type]=fn;}};
 const percent={textContent:'60 %'};
 const ui=new Card();ui.setConfig({title:'Wecker'});ui.hass=hass;
 ui._mode='wizard';ui._step=2;ui._slot=0;ui._draft=ui._slotData(0);
 ui.shadowRoot.querySelectorAll=()=>[];
 ui.shadowRoot.querySelector=(selector)=>selector==='[data-field="volume"]'?slider:selector==='[data-volume-label]'?percent:null;
 ui._render();
 assert(ui.shadowRoot.innerHTML.includes('data-volume-label'), 'Percentage output missing from settings wizard');
 assert.strictEqual(typeof slider.listeners.input, 'function', 'Live input event must be bound');
 slider.value='90';slider.listeners.input();
 assert.strictEqual(ui._draft.volume,90);
 assert.strictEqual(percent.textContent,'90 %');
 slider.value='10';slider.listeners.input();
 assert.strictEqual(ui._draft.volume,10);
 assert.strictEqual(percent.textContent,'10 %');
 assert(ui.shadowRoot.innerHTML.includes('Maximallautstärke'), 'Wizard should not be re-rendered during drag');
 console.log('HA card smoke test OK: add/edit/delete, time, occupied state and live volume label');
})().catch(err=>{console.error(err);process.exitCode=1;});

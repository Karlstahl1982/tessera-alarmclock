// Runtime smoke-test for the dependency-free Home Assistant card, without a browser.
const fs=require('fs'),vm=require('vm'),assert=require('assert'),yamlIgnore=void 0;
const registration=new Map();
global.HTMLElement=class {attachShadow(){return {innerHTML:'',querySelectorAll(){return []}};}};
global.customElements={define:(n,c)=>registration.set(n,c),get:n=>registration.get(n)};
global.window={customCards:[]};
global.confirm=()=>true;
vm.runInThisContext(fs.readFileSync(require('path').join(__dirname,'../ha_card/tessera-alarmclock-card.js'),'utf8'));
const Card=registration.get('tessera-alarmclock-card');
assert(Card,'Custom card is registered');
const states={};
function add(domain,id,label,state){states[domain+'.'+id]={entity_id:domain+'.'+id,state:String(state),attributes:{friendly_name:'Andy '+label}};}
for(let i=1;i<=5;i++){
 for(const [domain,key,val] of [['datetime','Uhrzeit','07:00:00'],['datetime','Datum','2026-11-04'],['switch','aktiv','off'],['select','Modus','Next'],['select','Klang','Gentle'],['select','Schlummerminuten','5'],['number','Lautstärke','60'],['text','Wochentage','Mo,Di,Mi,Do,Fr'],['text','Name','Wecker '+i]]){
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
  if(domain==='datetime')ent.state=body.time||body.date;
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
 assert.deepStrictEqual(c._occupied(),[false,false,false,false,false]);
 c._slot=-1;c._mode='wizard';c._draft=c._slotData(0);c._draft.name='Schule';c._draft.time='06:45';
 c._draft.mode='Weekly';c._draft.days=['Mo','Di','Mi','Do','Fr'];c._draft.enabled=true;
 await c._save();
 assert(states['sensor.ac_occupied'].state==='10000');
 assert(states['switch.ac_aktiv_1'].state==='on');
 assert(states['text.ac_name_1'].state==='Schule');
 assert(states['datetime.ac_uhrzeit_1'].state==='06:45:00');
 assert(calls.some(([domain,service])=>domain==='datetime'&&service==='set_value'));
 c._slot=0;c._mode='wizard';c._draft=c._slotData(0);
 await c._remove();
 assert(states['sensor.ac_occupied'].state==='00000');
 assert(states['switch.ac_aktiv_1'].state==='off');
 console.log('HA card smoke test OK: add/edit/delete, time and occupied state');
})().catch(err=>{console.error(err);process.exitCode=1;});

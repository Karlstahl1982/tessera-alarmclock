// HA card regression: only central operations, no per-alarm control.
const assert=require('assert'),fs=require('fs'),vm=require('vm'),path=require('path');
const registration=new Map();
global.HTMLElement=class { attachShadow(){this.shadowRoot={innerHTML:'',querySelectorAll(){return []}};return this.shadowRoot;} };
global.customElements={get:n=>registration.get(n),define:(n,c)=>registration.set(n,c)};
global.window={customCards:[]};
vm.runInThisContext(fs.readFileSync(path.join(__dirname,'../ha_card/tessera-alarmclock-card.js'),'utf8'));
const Card=registration.get('tessera-alarmclock-card');
assert(Card);
const states={
 'sensor.next':{entity_id:'sensor.next',state:'Schule · 07:00',attributes:{friendly_name:'Hilde AlarmClock Nächster Wecker'}},
 'switch.suppress':{entity_id:'switch.suppress',state:'off',attributes:{friendly_name:'Hilde AlarmClock Wecker unterdrücken'}},
 'button.stop':{entity_id:'button.stop',state:'unknown',attributes:{friendly_name:'Hilde AlarmClock Alarm stoppen'}},
};
const calls=[];
const hass={states,async callService(domain,service,data){calls.push([domain,service,data]);if(data.entity_id==='switch.suppress')states['switch.suppress'].state=service==='turn_on'?'on':'off';}};
(async()=>{
 const card=new Card();card.setConfig({title:'Hildes Wecker'});card.hass=hass;
 assert(card.shadowRoot.innerHTML.includes('Schule · 07:00'));
 assert(card.shadowRoot.innerHTML.includes('Wecker unterdrücken'));
 assert(!card.shadowRoot.innerHTML.includes('Wecker 1 Lautstärke'));
 assert(!card.shadowRoot.innerHTML.includes('data-action="edit"'));
 await card._action('mute');
 assert.equal(states['switch.suppress'].state,'on');
 assert(card.shadowRoot.innerHTML.includes('Wecker wieder zulassen'));
 await card._action('stop');
 await card._action('mute');
 assert.deepStrictEqual(calls.map(([d,s])=>[d,s]),[['switch','turn_on'],['button','press'],['switch','turn_off']]);
 console.log('HA card smoke test OK: only central controls, mute/unmute, stop');
})().catch(e=>{console.error(e);process.exitCode=1;});

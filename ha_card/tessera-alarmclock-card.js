/* Tessera AlarmClock v0.2.3 — dependency-free Home Assistant Lovelace card.
 * The ESP32 remains the source of truth. Only native ESPHome entities are used.
 */
class TesseraAlarmClockCard extends HTMLElement {
  constructor() {
    super();
    this.attachShadow({mode:'open'});
    this._mode='list';
    this._step=0;
    this._slot=-1;
    this._draft=null;
    this._busy=false;
    this._error='';
  }
  setConfig(config) {
    this._config={title:'Wecker',...config};
    this._render();
  }
  set hass(value) {
    this._hass=value;
    // Rebuilding a form while typing would lose cursor position and input.
    if (this._mode==='list') this._render();
  }
  getCardSize(){return 5;}
  _find(label) {
    const filter=(this._config?.device_filter||'').toLowerCase();
    return Object.values(this._hass?.states||{}).find(s=>{
      const name=String(s.attributes?.friendly_name||'');
      return name.endsWith(label) && (!filter || s.entity_id.includes(filter));
    })||null;
  }
  _state(label, fallback='') {return this._find(label)?.state??fallback;}
  _occupied() {
    const mask=this._state('AlarmClock Belegung');
    if(/^[01]{5}$/.test(mask)) return Array.from(mask,c=>c==='1');
    // Only a migration fallback; slot occupancy should normally come from ESP32.
    return Array.from({length:5},(_,i)=>this._state(`AlarmClock ${i+1} aktiv`)==='on');
  }
  _slotData(i){
    const k=i+1;
    const time=this._state(`AlarmClock ${k} Uhrzeit`,'07:00:00');
    const date=this._state(`AlarmClock ${k} Datum`,'2026-11-04');
    return {
      index:i, name:this._state(`AlarmClock ${k} Name`,`Wecker ${k}`),
      time:time.slice(0,5), date, enabled:this._state(`AlarmClock ${k} aktiv`)==='on',
      mode:this._state(`AlarmClock ${k} Modus`,'Next'),
      tone:this._state(`AlarmClock ${k} Klang`,'Gentle'),
      volume:parseFloat(this._state(`AlarmClock ${k} Lautstärke`,'60'))||60,
      snooze:this._state(`AlarmClock ${k} Schlummerminuten`,'5'),
      days:this._state(`AlarmClock ${k} Wochentage`,'Mo,Di,Mi,Do,Fr').split(',').filter(Boolean),
    };
  }
  _esc(v) {return String(v??'').replace(/[&<>"']/g,ch=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[ch]));}
  _modeLabel(a) {return a.mode==='Weekly'?(a.days.length?a.days.join(' · '):'Keine Wochentage'):a.mode==='Date'?`Einmalig · ${a.date}`:'Einmalig · nächste Uhrzeit';}
  _nextDue(a) {
    const now=new Date();
    const [h,m]=a.time.split(':').map(Number);
    if(!Number.isInteger(h)||!Number.isInteger(m))return Infinity;
    if(a.mode==='Date') {
      const when=new Date(`${a.date}T${a.time}:00`);
      return when.getTime()>now.getTime()?when.getTime():Infinity;
    }
    for(let n=0;n<8;n++) {
      const candidate=new Date(now);
      candidate.setDate(now.getDate()+n);
      candidate.setHours(h,m,0,0);
      if(candidate<=now)continue;
      if(a.mode==='Weekly') {
        const day=['Mo','Di','Mi','Do','Fr','Sa','So'][(candidate.getDay()+6)%7];
        if(!a.days.includes(day))continue;
      }
      return candidate.getTime();
    }
    return Infinity;
  }
  _style(){return `<style>
:host{display:block;color:var(--primary-text-color)}ha-card{padding:18px 20px 22px;border-radius:24px;overflow:hidden}
.head{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:18px}
.head h2{margin:0;font-size:23px;font-weight:690;letter-spacing:-.7px}
.muted{color:var(--secondary-text-color);font-size:13px}
.badge{padding:5px 10px;border-radius:100px;background:var(--secondary-background-color);font-size:12px}
.next{margin:0 0 16px;padding:20px 21px;border-radius:20px;background:var(--secondary-background-color)}
.next .clock{font-size:clamp(40px,7vw,62px);font-weight:300;letter-spacing:-2.5px;line-height:1.15}
.next .context{margin:7px 0 0;font-size:14px;color:var(--secondary-text-color)}
.alarms{display:grid;gap:9px}.alarm{display:flex;align-items:center;gap:12px;background:var(--card-background-color);border:1px solid var(--divider-color);border-radius:15px;padding:13px 12px}
.alarm .title{font-size:18px;font-weight:600}.alarm .clock{font-size:25px;min-width:82px;font-weight:350;font-variant-numeric:tabular-nums}
.alarm .detail{flex:1;min-width:0}.alarm .meta{font-size:12px;color:var(--secondary-text-color);white-space:nowrap;text-overflow:ellipsis;overflow:hidden}
button{font:inherit;color:inherit;border:0;cursor:pointer}button:disabled{opacity:.35;cursor:default}
.btn{padding:10px 13px;border-radius:12px;background:var(--secondary-background-color);font-weight:600}
.btn.primary{background:var(--primary-color);color:white}.btn.subtle{background:transparent;color:var(--primary-color)}
.btn.full{width:100%;margin-top:14px}.group{display:grid;gap:12px}.field{display:grid;gap:5px;font-size:13px;font-weight:600}
input[type=text],input[type=time],input[type=date],select{box-sizing:border-box;width:100%;background:var(--card-background-color);color:var(--primary-text-color);border:1px solid var(--divider-color);border-radius:12px;padding:13px;font:inherit;font-size:17px}
input[type=time]{font-size:32px;text-align:center;font-variant-numeric:tabular-nums}
input[type=range]{width:100%;accent-color:var(--primary-color)}
.choice{padding:15px 14px;border-radius:13px;background:var(--secondary-background-color);text-align:left;font-size:15px}
.choice.selected{outline:2px solid var(--primary-color);color:var(--primary-color)}
.days{display:grid;grid-template-columns:repeat(7,minmax(0,1fr));gap:4px}.day{min-width:0;padding:10px 0;border-radius:12px;background:var(--secondary-background-color);font-size:12px}
.day.selected{background:var(--primary-color);color:white}.actions{display:flex;gap:8px;justify-content:space-between;margin-top:20px}
.switch{width:42px;height:25px;border-radius:30px;background:var(--disabled-text-color);position:relative;flex:none;transition:background .15s}
.switch:after{content:'';position:absolute;left:3px;top:3px;width:19px;height:19px;border-radius:50%;background:white;transition:transform .15s}.switch.on{background:var(--success-color,#36b37e)}.switch.on:after{transform:translateX(17px)}
.step{font-size:12px;color:var(--secondary-text-color);margin-bottom:13px}.summary{display:grid;gap:9px;padding:14px;border-radius:14px;background:var(--secondary-background-color)}
.message{border-radius:12px;padding:11px;margin-top:12px;background:var(--error-color,#cc3333);color:white;font-size:13px}
.empty{padding:20px;text-align:center;color:var(--secondary-text-color)}
/* Graphic edit surface: readable on a narrow tablet and a wide HA dashboard. */
.edit-hero{display:grid;grid-template-columns: minmax(140px,1fr) 1fr;align-items:center;gap:16px;margin-bottom:16px;padding:20px;border-radius:22px;background:var(--secondary-background-color)}
.edit-hero .big-time{font-size:clamp(36px,7vw,64px);font-weight:330;line-height:1;letter-spacing:-2px;min-width:0}
.edit-hero input[type=time]{font-size:clamp(26px,5vw,42px);border:0;background:transparent;padding:6px 0}
.edit-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:12px}
.edit-panel{border:1px solid var(--divider-color);border-radius:17px;padding:17px;min-width:0}
.edit-panel-title{font-size:15px;font-weight:650;display:flex;align-items:center;gap:8px;margin-bottom:12px}
.edit-panel-title .symbol{display:inline-flex;align-items:center;justify-content:center;width:31px;height:31px;border-radius:10px;background:var(--secondary-background-color)}
.edit-panel.wide{grid-column:1/-1}.edit-choices{display:flex;flex-wrap:wrap;gap:8px}
.edit-choices .choice{flex:1 1 auto;text-align:center;padding:11px 13px}
.edit-tools{display:flex;gap:10px;align-items:center;justify-content:flex-end;flex-wrap:wrap;margin-top:19px}
.btn.danger{color:var(--error-color,#c73442);background:transparent}
@media(max-width:640px){.edit-grid{grid-template-columns:1fr}.edit-hero{grid-template-columns:1fr}.edit-hero input[type=time]{font-size:35px}}
@media(max-width:430px){ha-card{padding:14px}.alarm{gap:8px;padding:10px}.alarm .clock{font-size:21px;min-width:69px}.alarms{gap:6px}}
</style>`;}
  _render() {
    if(!this.shadowRoot || !this._config) return;
    const slots=this._occupied();
    const active=Array.from({length:5},(_,i)=>this._slotData(i)).filter((a,i)=>slots[i]&&a.enabled);
    const next=active.sort((a,b)=>this._nextDue(a)-this._nextDue(b)).find(a=>Number.isFinite(this._nextDue(a)));
    let content='';
    if(this._mode==='list') {
      content=`<div class="head"><h2>${this._esc(this._config.title)}</h2><span class="badge">${slots.filter(Boolean).length} / 5</span></div>
        <div class="next"><div class="muted">Nächster aktivierter Wecker</div><div class="clock">${this._esc(next?.time||'--:--')}</div>
        <div class="context">${next?this._esc(next.name)+' · '+this._esc(this._modeLabel(next)):'Kein Wecker aktiv'}</div></div>
        <div class="alarms">${Array.from({length:5},(_,i)=>slots[i]?this._alarmRow(this._slotData(i)):'').join('')}</div>
        ${slots.some(x=>!x)?'<button class="btn primary full" data-action="add">+ Neuen Wecker hinzufügen</button>':''}
        <div class="actions"><button class="btn subtle" data-action="test">Testton</button><button class="btn subtle" data-action="stop">Alarm stoppen</button></div>`;
    } else {
      content=this._mode==='edit'?this._editor():this._wizard();
    }
    this.shadowRoot.innerHTML=this._style()+`<ha-card>${content}${this._error?`<div class="message">${this._esc(this._error)}</div>`:''}</ha-card>`;
    this.shadowRoot.querySelectorAll('[data-action]').forEach(el=>el.addEventListener('click',e=>this._action(e.currentTarget)));
    // Keep the visible percentage in sync while dragging; re-rendering here
    // would replace the focused input and interrupt the drag gesture.
    const volume=this.shadowRoot.querySelector('[data-field="volume"]');
    if(volume) volume.addEventListener('input',()=>{
      if(!this._draft) return;
      this._draft.volume=Number(volume.value);
      const label=this.shadowRoot.querySelector('[data-volume-label]');
      if(label) label.textContent=`${Math.round(this._draft.volume)} %`;
    });
  }
  _alarmRow(a){return `<div class="alarm"><div class="clock">${this._esc(a.time)}</div><div class="detail"><div class="title">${this._esc(a.name)}</div><div class="meta">${this._esc(this._modeLabel(a))}</div></div><button class="btn subtle" data-action="edit" data-index="${a.index}" aria-label="Wecker bearbeiten">Bearbeiten</button><button class="switch ${a.enabled?'on':''}" data-action="toggle" data-index="${a.index}" aria-label="Wecker ${a.enabled?'deaktivieren':'aktivieren'}"></button></div>`;}
  _editor(){
    const d=this._draft; if(!d) return '';
    const weekdays=['Mo','Di','Mi','Do','Fr','Sa','So'];
    return `<div class="head"><h2>Wecker bearbeiten</h2><button class="btn subtle" data-action="cancel">Schließen</button></div>
      <div class="edit-hero">
        <div><div class="muted">Weckzeit</div><label class="field"><input aria-label="Weckzeit" data-field="time" type="time" step="60" value="${this._esc(d.time)}"/></label></div>
        <label class="field">Name<input data-field="name" type="text" maxlength="24" value="${this._esc(d.name)}" placeholder="z. B. Schule"/></label>
      </div>
      <div class="edit-grid">
        <section class="edit-panel wide"><div class="edit-panel-title"><span class="symbol">↻</span>Wiederholung</div>
          <div class="edit-choices">${[['Next','Nächste Uhrzeit'],['Date','Bestimmtes Datum'],['Weekly','Wöchentlich']].map(([v,t])=>`<button class="choice ${d.mode===v?'selected':''}" data-action="set-mode" data-value="${v}">${t}</button>`).join('')}</div>
          ${d.mode==='Date'?`<label class="field" style="margin-top:12px">Datum<input data-field="date" type="date" value="${this._esc(d.date)}"/></label>`:''}
          ${d.mode==='Weekly'?`<div class="days" style="margin-top:12px">${weekdays.map(day=>`<button class="day ${d.days.includes(day)?'selected':''}" data-action="day" data-value="${day}">${day}</button>`).join('')}</div>`:''}
        </section>
        <section class="edit-panel"><div class="edit-panel-title"><span class="symbol">♫</span>Alarmton</div>
          <label class="field">Melodie<select data-field="tone">${['Classic','Gentle','Digital','Sunrise','Urgent'].map(t=>`<option ${d.tone===t?'selected':''}>${t}</option>`).join('')}</select></label>
        </section>
        <section class="edit-panel"><div class="edit-panel-title"><span class="symbol">◖</span>Lautstärke</div>
          <label class="field">Maximal: <output data-volume-label>${Math.round(d.volume)} %</output><input data-field="volume" type="range" min="10" max="100" step="10" value="${d.volume}"/></label>
        </section>
        <section class="edit-panel"><div class="edit-panel-title"><span class="symbol">◷</span>Schlummern</div>
          <div class="edit-choices">${['5','10'].map(v=>`<button class="choice ${d.snooze===v?'selected':''}" data-action="snooze-choice" data-value="${v}">${v} Minuten</button>`).join('')}</div>
        </section>
        <section class="edit-panel"><div class="edit-panel-title"><span class="symbol">✓</span>Status</div>
          <div class="edit-choices">${[['on','Aktiv'],['off','Aus']].map(([v,t])=>`<button class="choice ${d.enabled===(v==='on')?'selected':''}" data-action="enabled-choice" data-value="${v}">${t}</button>`).join('')}</div>
        </section>
      </div>
      <div class="edit-tools"><button class="btn danger" data-action="delete">Löschen</button><button class="btn" data-action="cancel">Abbrechen</button><button class="btn primary" data-action="save-edit" ${this._busy?'disabled':''}>${this._busy?'Speichere …':'Änderungen speichern'}</button></div>`;
  }
  _wizard(){
    const d=this._draft; if(!d) return '';
    const labels=['Uhrzeit & Name','Wiederholung','Ton & Einstellungen','Überprüfen'];
    let body='';
    if(this._step===0) body=`<div class="group"><label class="field">Name<input data-field="name" type="text" maxlength="24" value="${this._esc(d.name)}" placeholder="z. B. Schule"/></label>
      <label class="field">Uhrzeit<input data-field="time" type="time" step="60" value="${this._esc(d.time)}"/></label></div>`;
    if(this._step===1) body=`<div class="group"><div class="muted">Wie oft soll dieser Wecker klingeln?</div>
      ${[['Next','Einmalig · nächste Uhrzeit'],['Date','Einmalig · bestimmtes Datum'],['Weekly','Wöchentlich']].map(([v,t])=>`<button class="choice ${d.mode===v?'selected':''}" data-action="set-mode" data-value="${v}">${t}</button>`).join('')}
      ${d.mode==='Date'?`<label class="field">Datum<input data-field="date" type="date" value="${this._esc(d.date)}"/></label>`:''}
      ${d.mode==='Weekly'?`<div class="days">${['Mo','Di','Mi','Do','Fr','Sa','So'].map(day=>`<button class="day ${d.days.includes(day)?'selected':''}" data-action="day" data-value="${day}">${day}</button>`).join('')}</div>`:''}</div>`;
    if(this._step===2) body=`<div class="group"><label class="field">Alarmton<select data-field="tone">${['Classic','Gentle','Digital','Sunrise','Urgent'].map(t=>`<option ${d.tone===t?'selected':''}>${t}</option>`).join('')}</select></label>
      <label class="field">Maximallautstärke: <output data-volume-label aria-live="off">${Math.round(d.volume)} %</output><input data-field="volume" type="range" min="10" max="100" step="10" value="${d.volume}"/></label>
      <label class="field">Schlummern<select data-field="snooze"><option value="5" ${d.snooze==='5'?'selected':''}>5 Minuten</option><option value="10" ${d.snooze==='10'?'selected':''}>10 Minuten</option></select></label>
      <label class="field">Status<select data-field="enabled"><option value="on" ${d.enabled?'selected':''}>Aktiv</option><option value="off" ${!d.enabled?'selected':''}>Deaktiviert</option></select></label></div>`;
    if(this._step===3) body=`<div class="summary"><div style="font-size:35px;font-weight:300">${this._esc(d.time)}</div><strong>${this._esc(d.name)}</strong><span>${this._esc(this._modeLabel(d))}</span><span>${this._esc(d.tone)} · ${d.volume} % · Schlummern ${d.snooze} Min.</span><span>${d.enabled?'Aktiviert':'Deaktiviert'}</span></div>`;
    return `<div class="head"><h2>${this._slot<0?'Neuer Wecker':'Wecker bearbeiten'}</h2><button class="btn subtle" data-action="cancel">Schließen</button></div>
      <div class="step">Schritt ${this._step+1} von 4 · ${labels[this._step]}</div>${body}
      <div class="actions"><button class="btn" data-action="back" ${this._busy?'disabled':''}>${this._step?'Zurück':'Abbrechen'}</button>
      <button class="btn primary" data-action="next" ${this._busy?'disabled':''}>${this._busy?'Speichere …':this._step===3?'Wecker speichern':'Weiter'}</button></div>
      ${this._slot>=0?'<button class="btn subtle full" data-action="delete" style="margin-top:14px">Wecker löschen</button>':''}`;
  }
  _collect(){
    if(!this._draft) return;
    for(const input of this.shadowRoot.querySelectorAll('[data-field]')) {
      const key=input.dataset.field;
      if(key==='volume') this._draft[key]=Number(input.value);
      else if(key==='enabled') this._draft.enabled=input.value==='on';
      else this._draft[key]=input.value;
    }
  }
  async _action(button){
    const action=button.dataset.action;
    this._error='';
    if(this._busy) return;
    try {
      if(action==='add') {
        this._slot=-1; this._draft={...this._slotData(this._occupied().findIndex(x=>!x)),enabled:true};
        this._step=0;this._mode='wizard';
      } else if(action==='edit') {
        this._slot=Number(button.dataset.index);this._draft=this._slotData(this._slot);this._step=0;this._mode='edit';
      } else if(action==='cancel') {this._mode='list';this._draft=null;}
      else if(action==='save-edit') {this._collect();await this._save();}
      else if(action==='snooze-choice') {this._collect();this._draft.snooze=button.dataset.value;}
      else if(action==='enabled-choice') {this._collect();this._draft.enabled=button.dataset.value==='on';}
      else if(action==='back') {this._collect();if(this._step===0)this._mode='list';else this._step--;}
      else if(action==='next') {this._collect();if(this._step<3){this._step++;}else await this._save();}
      else if(action==='set-mode') {this._collect();this._draft.mode=button.dataset.value;}
      else if(action==='day') {this._collect();const day=button.dataset.value;const a=this._draft.days;this._draft.days=a.includes(day)?a.filter(x=>x!==day):[...a,day];}
      else if(action==='toggle') {const i=Number(button.dataset.index);await this._service('switch',this._state(`AlarmClock ${i+1} aktiv`)==='on'?'turn_off':'turn_on',`AlarmClock ${i+1} aktiv`);}
      else if(action==='delete') {if(confirm('Diesen Wecker wirklich löschen?'))await this._remove();}
      else if(action==='test') await this._service('button','press','AlarmClock Testalarm');
      else if(action==='stop') await this._service('button','press','AlarmClock Alarm stoppen');
    } catch(e){this._error=e.message||String(e);}
    this._render();
  }
  async _service(domain,service,label,values={}) {
    const found=this._find(label);
    if(!found) throw new Error(`Entität fehlt: ${label}. Firmware und ESPHome-Integration prüfen.`);
    await this._hass.callService(domain,service,{entity_id:found.entity_id,...values});
  }
  async _save(){
    const d=this._draft;
    const i=(this._slot<0?this._occupied().findIndex(v=>!v):this._slot)+1;
    if(i<1)throw new Error('Alle fünf Plätze sind belegt.');
    if(!/^[0-2]\d:[0-5]\d$/.test(d.time)||Number(d.time.slice(0,2))>23)throw new Error('Ungültige Uhrzeit.');
    if(!d.name.trim())throw new Error('Bitte einen Namen eingeben.');
    if(d.mode==='Weekly'&&!d.days.length)throw new Error('Mindestens einen Wochentag wählen.');
    this._busy=true;
    try{
      // Validate all needed entities BEFORE changing anything.
      const requirements=[['AlarmClock '+i+' Uhrzeit'],['AlarmClock '+i+' Datum'],['AlarmClock '+i+' aktiv'],['AlarmClock '+i+' Name'],['AlarmClock '+i+' Modus'],['AlarmClock '+i+' Klang'],['AlarmClock '+i+' Lautstärke'],['AlarmClock '+i+' Schlummerminuten'],['AlarmClock '+i+' Wochentage'],['AlarmClock Verwaltung']];
      for(const [label] of requirements)if(!this._find(label))throw new Error('Entität fehlt: '+label);
      // Fail before altering any alarm: ESPHome template datetime(type:time/date)
      // is exposed as time.* / date.* in Home Assistant, not datetime.*.
      const expectedDomains=[
        [`AlarmClock ${i} Uhrzeit`,'time'],[`AlarmClock ${i} Datum`,'date'],
        [`AlarmClock ${i} aktiv`,'switch'],[`AlarmClock ${i} Name`,'text'],
        [`AlarmClock ${i} Modus`,'select'],[`AlarmClock ${i} Klang`,'select'],
        [`AlarmClock ${i} Lautstärke`,'number'],[`AlarmClock ${i} Schlummerminuten`,'select'],
        [`AlarmClock ${i} Wochentage`,'text'],['AlarmClock Verwaltung','text']
      ];
      for(const [label,domain] of expectedDomains){
        const actual=this._find(label)?.entity_id.split('.')[0];
        if(actual!==domain)throw new Error(`Entität ${label}: erwartet ${domain}, gefunden ${actual||'keine'}.`);
      }
      await this._service('switch','turn_off',`AlarmClock ${i} aktiv`);
      await this._service('time','set_value',`AlarmClock ${i} Uhrzeit`,{time:d.time+':00'});
      await this._service('date','set_value',`AlarmClock ${i} Datum`,{date:d.date});
      await this._service('text','set_value',`AlarmClock ${i} Name`,{value:d.name.slice(0,24)});
      await this._service('select','select_option',`AlarmClock ${i} Modus`,{option:d.mode});
      await this._service('select','select_option',`AlarmClock ${i} Klang`,{option:d.tone});
      await this._service('number','set_value',`AlarmClock ${i} Lautstärke`,{value:d.volume});
      await this._service('select','select_option',`AlarmClock ${i} Schlummerminuten`,{option:d.snooze});
      await this._service('text','set_value',`AlarmClock ${i} Wochentage`,{value:d.days.join(',')});
      await this._service('text','set_value','AlarmClock Verwaltung',{value:'add:'+i});
      if(d.enabled)await this._service('switch','turn_on',`AlarmClock ${i} aktiv`);
      this._draft=null;this._mode='list';this._slot=-1;
    }finally{this._busy=false;}
  }
  async _remove(){
    const i=this._slot+1;
    this._busy=true;
    try{
      await this._service('switch','turn_off',`AlarmClock ${i} aktiv`);
      await this._service('text','set_value','AlarmClock Verwaltung',{value:'remove:'+i});
      this._mode='list';this._draft=null;this._slot=-1;
    }finally{this._busy=false;}
  }
}
if(!customElements.get('tessera-alarmclock-card'))customElements.define('tessera-alarmclock-card',TesseraAlarmClockCard);
window.customCards=window.customCards||[];
window.customCards.push({type:'tessera-alarmclock-card',name:'Tessera AlarmClock',description:'5 lokale ESP32-Wecker mit übersichtlicher HA-Bedienung'});

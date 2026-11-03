/* Tessera AlarmClock 0.3.4 — zentrale Home-Assistant-Steuerung.
 * Weckerzeiten und Einstellungen bleiben ausschließlich auf dem ESP32.
 */
class TesseraAlarmClockCard extends HTMLElement {
  constructor() {
    super();
    this.attachShadow({mode:'open'});
    this._pending=false;
    this._error='';
  }
  setConfig(config) { this._config={title:'Wecker',...config}; this._render(); }
  set hass(value) { this._hass=value; this._render(); }
  getCardSize() {return 3;}
  _entity(suffix) {
    const filter=String(this._config?.device_filter||'').toLowerCase();
    return Object.values(this._hass?.states||{}).find(s=>{
      const name=String(s.attributes?.friendly_name||'');
      return name.endsWith(suffix)&&(!filter||s.entity_id.toLowerCase().includes(filter));
    })||null;
  }
  _escape(s){return String(s??'').replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));}
  _render() {
    if(!this._config||!this.shadowRoot) return;
    const next=this._entity('AlarmClock Nächster Wecker');
    const mute=this._entity('AlarmClock Wecker unterdrücken');
    const stop=this._entity('AlarmClock Alarm stoppen');
    const suppressed=mute?.state==='on';
    const status=next?.state&&!['unknown','unavailable'].includes(next.state)?next.state:'Keine Statusdaten verfügbar';
    this.shadowRoot.innerHTML=`<style>
      :host{display:block;color:var(--primary-text-color)}
      ha-card{padding:20px;border-radius:22px}
      .head{display:flex;align-items:center;justify-content:space-between;margin-bottom:16px;gap:12px}
      h2{font-size:22px;margin:0;font-weight:650}
      .status{padding:22px;border-radius:18px;background:var(--secondary-background-color)}
      .caption{font-size:13px;color:var(--secondary-text-color);margin-bottom:9px}
      .value{font-size:clamp(19px,3.5vw,29px);font-weight:550;line-height:1.35;overflow-wrap:anywhere}
      .actions{display:flex;gap:10px;flex-wrap:wrap;margin-top:16px}
      button{font:inherit;cursor:pointer;border:0;border-radius:12px;padding:13px 15px;font-weight:650}
      button.primary{background:var(--primary-color);color:var(--text-primary-color,#fff);flex:1}
      button.secondary{background:var(--secondary-background-color);color:var(--primary-text-color);flex:1}
      button:disabled{opacity:.45;cursor:not-allowed}
      .flag{font-size:12px;padding:7px 10px;border-radius:18px;background:var(--secondary-background-color)}
      .flag.muted{background:var(--warning-color,#b27605);color:white}
      .note{font-size:12px;color:var(--secondary-text-color);margin-top:15px}
      .error{margin-top:12px;color:var(--error-color,#d22);font-size:13px}
    </style>
    <ha-card>
      <div class="head"><h2>${this._escape(this._config.title)}</h2><span class="flag ${suppressed?'muted':''}">${suppressed?'Unterdrückt':'Lokal aktiv'}</span></div>
      <div class="status"><div class="caption">Nächster Wecker / aktueller Zustand</div><div class="value">${this._escape(status)}</div></div>
      <div class="actions">
        <button class="${suppressed?'secondary':'primary'}" data-action="mute" ${!mute||this._pending?'disabled':''}>${suppressed?'Wecker wieder zulassen':'Wecker unterdrücken'}</button>
        <button class="secondary" data-action="stop" ${!stop||this._pending?'disabled':''}>Klingeln stoppen</button>
      </div>
      <div class="note">Alle fünf Wecker werden ausschließlich am Tessera-Display eingerichtet. Unterdrücken überspringt anstehende Alarme, bis du es ausschaltest.</div>
      ${this._error?`<div class="error">${this._escape(this._error)}</div>`:''}
    </ha-card>`;
    this.shadowRoot.querySelectorAll('[data-action]').forEach(el=>el.addEventListener('click',()=>this._action(el.dataset.action)));
  }
  async _action(action) {
    if(this._pending||!this._hass)return;
    const mute=this._entity('AlarmClock Wecker unterdrücken');
    const stop=this._entity('AlarmClock Alarm stoppen');
    this._pending=true;this._error='';this._render();
    try {
      if(action==='mute') {
        if(!mute)throw Error('Zentraler Schalter nicht gefunden. Firmware v0.3.4 erforderlich.');
        const service=mute.state==='on'?'turn_off':'turn_on';
        await this._hass.callService('switch',service,{entity_id:mute.entity_id});
      } else if(action==='stop') {
        if(!stop)throw Error('Stopptaste nicht gefunden.');
        await this._hass.callService('button','press',{entity_id:stop.entity_id});
      }
    } catch(error) {this._error=String(error?.message||error);}
    finally {this._pending=false;this._render();}
  }
}
if(!customElements.get('tessera-alarmclock-card')) customElements.define('tessera-alarmclock-card',TesseraAlarmClockCard);
window.customCards=window.customCards||[];
window.customCards.push({type:'tessera-alarmclock-card',name:'Tessera AlarmClock (zentrale Steuerung)',description:'Nur nächster Wecker, globale Unterdrückung und Stoppen; keine Einzel-Wecker-Konfiguration in Home Assistant.'});

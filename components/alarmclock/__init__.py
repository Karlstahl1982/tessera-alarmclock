"""Tessera AlarmClock: five autonomous alarms with ESPHome entities."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import datetime, number, rtttl, select, smart_display, speaker, switch, text, text_sensor
from esphome.const import CONF_ID

DEPENDENCIES = ["smart_display"]
AC_NS = cg.esphome_ns.namespace("alarmclock")
AlarmClock = AC_NS.class_("AlarmClock", cg.Component)
CONF_TONE = "tone"
CONF_SPEAKER = "speaker"
FIELDS = {
    "time": datetime.TimeEntity,
    "date": datetime.DateEntity,
    "enabled": switch.Switch,
    "mode": select.Select,
    "tone": select.Select,
    "snooze": select.Select,
    "volume": number.Number,
    "days": text.Text,
    "name": text.Text,
}
_schema = {cv.GenerateID(): cv.declare_id(AlarmClock), cv.Required(CONF_TONE): cv.use_id(rtttl.Rtttl), cv.Required(CONF_SPEAKER): cv.use_id(speaker.Speaker)}
_schema[cv.Required("occupied_sensor")] = cv.use_id(text_sensor.TextSensor)
for index in range(1, 6):
    for field, typ in FIELDS.items():
        _schema[cv.Required(f"{field}_{index}")] = cv.use_id(typ)
CONFIG_SCHEMA = cv.Schema(_schema).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await smart_display.register_plugin(var, __file__)
    cg.add(var.set_tone_player(await cg.get_variable(config[CONF_TONE])))
    cg.add(var.set_speaker(await cg.get_variable(config[CONF_SPEAKER])))
    cg.add(var.set_occupied_sensor(await cg.get_variable(config["occupied_sensor"])))
    for i in range(1, 6):
        for field in FIELDS:
            cg.add(getattr(var, f"set_{field}")(i - 1, await cg.get_variable(config[f"{field}_{i}"])))

"""Tessera AlarmClock: five autonomous alarms with ESPHome entities."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import datetime, number, rtttl, select, smart_display, switch, text
from esphome.const import CONF_ID

DEPENDENCIES = ["smart_display"]
AC_NS = cg.esphome_ns.namespace("alarmclock")
AlarmClock = AC_NS.class_("AlarmClock", cg.Component)
CONF_TONE = "tone"
FIELDS = {
    "time": datetime.TimeEntity,
    "date": datetime.DateEntity,
    "enabled": switch.Switch,
    "mode": select.Select,
    "tone": select.Select,
    "snooze": select.Select,
    "volume": number.Number,
    "days": text.Text,
}
_schema = {cv.GenerateID(): cv.declare_id(AlarmClock), cv.Required(CONF_TONE): cv.use_id(rtttl.Rtttl)}
for index in range(1, 6):
    for field, typ in FIELDS.items():
        _schema[cv.Required(f"{field}_{index}")] = cv.use_id(typ)
CONFIG_SCHEMA = cv.Schema(_schema).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await smart_display.register_plugin(var, __file__)
    cg.add(var.set_tone_player(await cg.get_variable(config[CONF_TONE])))
    for i in range(1, 6):
        for field in FIELDS:
            cg.add(getattr(var, f"set_{field}")(i - 1, await cg.get_variable(config[f"{field}_{i}"])))

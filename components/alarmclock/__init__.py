"""Tessera AlarmClock: first local single-shot alarm with native ESPHome settings."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number, rtttl, smart_display, switch
from esphome.const import CONF_ID

DEPENDENCIES = ["smart_display"]

CONF_HOUR = "hour"
CONF_MINUTE = "minute"
CONF_ENABLED = "enabled"
CONF_TONE = "tone"

alarmclock_ns = cg.esphome_ns.namespace("alarmclock")
AlarmClock = alarmclock_ns.class_("AlarmClock", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(AlarmClock),
    cv.Required(CONF_HOUR): cv.use_id(number.Number),
    cv.Required(CONF_MINUTE): cv.use_id(number.Number),
    cv.Required(CONF_ENABLED): cv.use_id(switch.Switch),
    cv.Required(CONF_TONE): cv.use_id(rtttl.Rtttl),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await smart_display.register_plugin(var, __file__)
    cg.add(var.set_hour(await cg.get_variable(config[CONF_HOUR])))
    cg.add(var.set_minute(await cg.get_variable(config[CONF_MINUTE])))
    cg.add(var.set_enabled(await cg.get_variable(config[CONF_ENABLED])))
    cg.add(var.set_tone(await cg.get_variable(config[CONF_TONE])))

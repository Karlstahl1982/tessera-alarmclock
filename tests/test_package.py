"""Static release tests. They do not replace an ESPHome firmware build."""
from pathlib import Path
import json, re, subprocess, unittest, shutil
import yaml
ROOT=Path(__file__).resolve().parents[1]

class PackageTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.manifest=yaml.safe_load((ROOT/'tessera-plugin.yaml').read_text())
        cls.package=yaml.safe_load((ROOT/'plugin.yaml').read_text())
        cls.cpp=(ROOT/'components/alarmclock/alarmclock.cpp').read_text()
        cls.header=(ROOT/'components/alarmclock/alarmclock.h').read_text()
        cls.codegen=(ROOT/'components/alarmclock/__init__.py').read_text()

    def test_manifest(self):
        self.assertEqual(self.manifest['id'],'alarmclock')
        self.assertEqual(self.manifest['version'],'0.3.2')
        self.assertRegex(self.manifest['version'],r'^\d+\.\d+\.\d+$')
        self.assertEqual(self.manifest['tiles'][0]['sizes']['max'],'2x3')
        self.assertIn('overview', [card['id'] for card in self.manifest['cards']])

    def test_five_slots_and_entity_binds(self):
        p=self.package
        self.assertEqual(len(p['datetime']),10)
        self.assertEqual(len(p['switch']),5)
        self.assertEqual(len(p['select']),15)
        self.assertEqual(len(p['number']),5)
        self.assertEqual(len(p['text']),11)
        self.assertEqual(len(p['text_sensor']),1)
        for i in range(1,6):
            for field in ('time','date','enabled','mode','tone','snooze','volume','days','name'):
                entity=f'ac_{field}_{i}'
                self.assertEqual(p['alarmclock'][f'{field}_{i}'],entity)
                self.assertIn(f'"{field}"', self.codegen)
        self.assertEqual(p['alarmclock']['occupied_sensor'],'ac_occupied')
        all_ids=[e['id'] for section in ['datetime','switch','select','number','text','text_sensor'] for e in p[section]]
        self.assertEqual(len(all_ids),len(set(all_ids)))
        self.assertTrue(all(e.get('entity_category') in ('config','diagnostic') for section in ['datetime','switch','select','number','text','text_sensor'] for e in p[section]))

    def test_restore_and_audio_preserved(self):
        p=self.package
        self.assertEqual(p['alarmclock']['tone'],'guition_tone')
        self.assertEqual(p['alarmclock']['speaker'],'guition_speaker')
        self.assertEqual([v['initial_value'] for v in p['datetime'] if v['type']=='time'],
                         ['07:00:00','08:00:00','09:00:00','10:00:00','11:00:00'])
        self.assertTrue(all(v['restore_value'] for v in p['datetime']))
        self.assertTrue(all(v['restore_value'] for v in p['text'] if v['id'].startswith('ac_name_')))

    def test_translation_coverage(self):
        de=json.loads((ROOT/'translations/de.json').read_text())
        en=json.loads((ROOT/'translations/en.json').read_text())
        self.assertEqual(set(de['screen']),set(en['screen']))
        keys=set(re.findall(r'(?:p_->|this->)text\("([^\"]+)"\)', self.cpp))
        self.assertFalse(keys-set(de['screen']),keys-set(de['screen']))
        for key in ('add_alarm','edit_alarm','hero_title','step_type','step_time','step_sound','delete_alarm'):
            self.assertIn(key,de['screen'])

    def test_local_logic_and_wizard(self):
        for needle in ('enum Screen { LIST, EDIT, TYPE, TIME, DATE, DAYS, SOUND, OPTIONS, REVIEW, CONFIRM_DELETE }',
                       'void begin_new_()', 'void begin_edit_', 'void next_step_()',
                       'void save_()', 'void AlarmClock::set_occupied',
                       'void AlarmClock::publish_occupied_', 'occupied_preference_.save',
                       'void AlarmClock::preview_tone', 'void AlarmClock::on_interval',
                       'lv_roller_set_options(hour_', 'lv_roller_set_options(minute_',
                       'constexpr const char *SONGS[]'):
            self.assertIn(needle,self.cpp)


    def test_audio_cold_start_and_live_volume(self):
        c=self.cpp
        for token in ('const bool already_ringing=any_ringing();',
                      'play_current_(now_ms); // Start audio promptly',
                      'speaker_->set_volume(gain);',
                      'VolumeRamp::current_percent(', 'VolumeRamp::seconds_to_next(',
                      'last_audio_attempt_ms_', 'audio_start_retries_<2',
                      'overlay_countdown_', 'overlay_progress_fill_'):
            self.assertIn(token,c)
        self.assertNotIn('0.10f+0.90f',c)
        self.assertNotIn('0.15f * volume(audio_owner_)',c)

    def test_audio_ramp_runtime(self):
        if not shutil.which('g++'): self.skipTest('g++ missing')
        source=ROOT/'tests/test_audio_ramp.cpp'
        executable=ROOT/'tests/_volume_ramp_test'
        try:
            subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-I'+str(ROOT/'components/alarmclock'),str(source),'-o',str(executable)],check=True,capture_output=True,text=True)
            subprocess.run([str(executable)],check=True,capture_output=True,text=True)
        finally:
            if executable.exists(): executable.unlink()


    def test_audio_mock_runtime(self):
        if not shutil.which('g++'): self.skipTest('g++ missing')
        for name in ('test_audio_runtime','test_cold_start'):
            source=ROOT/f'tests/{name}.cpp'
            executable=ROOT/f'tests/_{name}'
            try:
                subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror',
                                '-I'+str(ROOT/'tests/cpp_mocks'),
                                '-I'+str(ROOT/'components/alarmclock'),
                                str(source),str(ROOT/'components/alarmclock/alarmclock.cpp'),
                                '-o',str(executable)],check=True,capture_output=True,text=True)
                subprocess.run([str(executable)],check=True,capture_output=True,text=True)
            finally:
                if executable.exists(): executable.unlink()

    def test_js_syntax(self):
        subprocess.run(['node','--check',str(ROOT/'ha_card/tessera-alarmclock-card.js')],check=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)

    def test_frontend_runtime(self):
        result=subprocess.run(['node',str(ROOT/'tests/test_frontend.js')],check=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
        self.assertIn('smoke test OK', result.stdout)

    def test_home_assistant_time_date_service_contract(self):
        js=(ROOT/'ha_card/tessera-alarmclock-card.js').read_text()
        self.assertIn("_service('time','set_value'",js)
        self.assertIn("_service('date','set_value'",js)
        self.assertNotIn("_service('datetime','set_value'",js)

    def test_graphic_edit_and_modal_alarm(self):
        c=self.cpp
        for fragment in (
            'screen_=EDIT;', 'if (screen_==EDIT)', 'screen_=TIME; init_time_wheels_();',
            'screen_=CONFIRM_DELETE;', 'footer_((screen_==REVIEW || screen_==EDIT)',
            'lv_obj_create(lv_layer_sys())', 'LV_OBJ_FLAG_CLICKABLE',
            'lv_obj_add_event_cb(btn,cb,LV_EVENT_CLICKED,this)',
            'void AlarmClock::on_overlay_stop_', 'void AlarmClock::on_overlay_snooze_',
            'lv_obj_delete(alarm_overlay_)', 's.audio_expired=true;',
            'if (any_ringing() && (alarm_overlay_==nullptr',
        ):
            self.assertIn(fragment,c)
        # No default-Tessera popup that closes upon background tap.
        self.assertNotIn('tessera::show_popup(',c)
        self.assertNotIn('tessera::popup_shown(',c)
        self.assertNotIn('tessera::close_popup(',c)
        self.assertNotIn('lv_button_create(', c, 'Do not require optional LVGL Button widget')
        self.assertIn('lv_obj_t *btn=lv_obj_create(alarm_overlay_);', c)
        self.assertIn('lv_obj_add_flag(btn,LV_OBJ_FLAG_CLICKABLE);', c)
        # A timeout must silence audio without auto-acknowledging the alarm.
        self.assertNotRegex(c,r'\(s\.test\?15000u:300000u\)\)\s*stop_alarm')
        js=(ROOT/'ha_card/tessera-alarmclock-card.js').read_text()
        for fragment in ('_editor(){', 'class="edit-hero"', 'class="edit-grid"',
                         'data-action="save-edit"', "this._mode='edit'", "action==='save-edit'"):
            self.assertIn(fragment,js)

    def test_display_grid_contract(self):
        # 2 columns x 3 rows, with an add tile and direct status toggle.
        for token in ('VisualCard list_cards_[6]', 'edit_cards_[4]',
                      'void paint_visual_', 'void make_visual_',
                      'const int index=row*2+col;', 'if (index==listed_',
                      'if (screen_==EDIT)', 'switch (row*2+col)',
                      'toggle_hidden(c.disc,false)', 'p_->text("grid_add")'):
            self.assertIn(token,self.cpp)
        # LVGL object construction happens at card-open only, never in rendering.
        render=self.cpp.split('void render_()',1)[1].split('AlarmClock *p_',1)[0]
        self.assertNotIn('lv_obj_create(',render)
        for lang in ('de','en'):
            labels=json.loads((ROOT/f'translations/{lang}.json').read_text())['screen']
            for key in ('grid_add','grid_time','grid_repeat','grid_audio','grid_tone',
                        'grid_on','grid_off','edit_short'):
                self.assertIn(key,labels)

    def test_graphical_detail_screens(self):
        for fragment in ('int detail_top_() const', 'int type_card_height_() const',
                         'int detail_grid_height_() const', 'int options_card_height_() const',
                         'if(screen_==TYPE)', 'if(screen_==DAYS)', 'if(screen_==SOUND)',
                         'if(screen_==OPTIONS)', 'preview_short', 'days_select',
                         'const int name_x=x+ui::px(39)', 'const int clock_h=',
                         'if(index<5) draft_.tone=index;', 'else p_->preview_tone(draft_.tone);'):
            self.assertIn(fragment,self.cpp)
        # Geometry and click areas are driven by the same layout functions.
        self.assertIn('const int row=(y-top)/(row_h+gap);',self.cpp)
        self.assertIn('card_h=detail_grid_height_();',self.cpp)
        self.assertIn('ch=options_card_height_();',self.cpp)
        render=self.cpp.split('void render_()',1)[1].split('AlarmClock *p_',1)[0]
        self.assertNotIn('lv_obj_create(',render)

    def test_cpp_mock_compilation(self):
        if not shutil.which('g++'): self.skipTest('g++ missing')
        subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-fsyntax-only',
                        '-I'+str(ROOT/'tests/cpp_mocks'),str(ROOT/'components/alarmclock/alarmclock.cpp')],
                       check=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)

    def test_frontend_resources(self):
        js=(ROOT/'ha_card/tessera-alarmclock-card.js').read_text()
        self.assertIn('class TesseraAlarmClockCard',js)
        self.assertIn("customElements.define('tessera-alarmclock-card'",js)
        self.assertIn("this._hass.callService",js)
        self.assertIn("this._nextDue",js)
        self.assertTrue((ROOT/'ha_card/LOVELACE_KARTE.yaml').exists())

if __name__=='__main__': unittest.main()

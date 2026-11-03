from pathlib import Path
import json, unittest, re, yaml
ROOT=Path(__file__).resolve().parents[1]
class PackageTest(unittest.TestCase):
    def test_schema_and_bindings(self):
        manifest=yaml.safe_load((ROOT/'tessera-plugin.yaml').read_text())
        plugin=yaml.safe_load((ROOT/'plugin.yaml').read_text())
        self.assertEqual(manifest['id'],'alarmclock')
        self.assertEqual(manifest['version'],'0.1.0')
        self.assertEqual(len(plugin['datetime']),10)
        self.assertEqual([p['initial_value'] for p in plugin['datetime'] if p['type']=='time'], ['07:00:00','08:00:00','09:00:00','10:00:00','11:00:00'])
        self.assertRegex(manifest['version'],r'^\d+\.\d+\.\d+$')
        self.assertEqual(len(plugin['switch']),5)
        self.assertEqual(len(plugin['select']),15)
        self.assertEqual(len(plugin['text']),5)
        self.assertEqual(len(plugin['number']),5)
        for i in range(1,6):
            for field in ('time','date','enabled','mode','tone','snooze','volume','days'):
                self.assertEqual(plugin['alarmclock'][f'{field}_{i}'],f'ac_{field}_{i}')
    def test_translation_coverage(self):
        de=json.loads((ROOT/'translations/de.json').read_text())
        en=json.loads((ROOT/'translations/en.json').read_text())
        self.assertEqual(set(de['screen']),set(en['screen']))
        cpp=(ROOT/'components/alarmclock/alarmclock.cpp').read_text()
        keys=set(re.findall(r'(?:p_->|this->)text\("([^"]+)"\)',cpp))
        self.assertFalse(keys-set(de['screen']),keys-set(de['screen']))
    def test_five_tunes(self):
        cpp=(ROOT/'components/alarmclock/alarmclock.cpp').read_text()
        self.assertIn('"classic:d=',cpp)
        self.assertIn('"urgent:d=',cpp)
        self.assertIn('lv_roller_set_options(hour_',cpp)
        self.assertIn('lv_roller_set_options(minute_',cpp)
if __name__=='__main__': unittest.main()

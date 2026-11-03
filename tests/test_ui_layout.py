"""Screen UI geometry regression checks (Guition 480x480 and smaller Tessera card heights)."""
import unittest

class UiLayout(unittest.TestCase):
    def test_list_card_typography_bands(self):
        for h in (360,380,480):
            top=53
            ch=(h-top-16-16)//3
            self.assertGreaterEqual(ch,85)
            compact=ch<112
            header=(4,28) if compact else (7,34)
            clock_top=31 if compact else 36
            clock_height=max(20,ch-61) if compact else ch-72
            clock=(clock_top,clock_top+clock_height)
            footer=(ch-26,ch-5)
            self.assertLessEqual(header[1],clock[0])
            self.assertLessEqual(clock[1],footer[0],(h,ch,clock,footer))

    def test_edit_card_bands(self):
        for h in (360,380,480):
            ch=((h-103)-139-8)//2
            self.assertGreaterEqual(ch,55)
            compact=ch<90
            title=(2,26) if compact else (8,37)
            value=(26,52) if compact else (max(42,ch-48),max(42,ch-48)+39)
            self.assertGreaterEqual(value[0],title[1])
            self.assertLessEqual(value[1],ch,(h,ch))

    def test_details_click_regions(self):
        for h in (360,380,480):
            t=70
            type_h=(h-t-62-2*9)//3
            sound_h=(h-t-61-2*8)//3
            option_h=(h-t-63-8)//2
            self.assertGreater(type_h,45)
            self.assertGreater(sound_h,45)
            self.assertGreater(option_h,70)
            self.assertLessEqual(t+3*type_h+2*9,h-60)
            self.assertLessEqual(t+3*sound_h+2*8,h-60)
            self.assertLessEqual(t+2*option_h+8,h-62)

if __name__=='__main__':unittest.main()

"""Geometry sanity checks for the 480x480 Guition's Tessera card viewport.

Approximates the geometry using ui::px(n) == n. This is NOT a device render test.
"""
import unittest

class GridGeometry(unittest.TestCase):
    def test_alarm_grid_various_viewport_heights(self):
        for w, h in ((480, 360), (480, 380), (480, 480)):
            margin, gap, top = 8, 8, 63
            cw=(w-2*margin-gap)//2
            ch=(h-top-16-2*gap)//3
            boxes=[]
            for i in range(6):
                x=margin+(i%2)*(cw+gap)
                y=top+(i//2)*(ch+gap)
                boxes.append((x,y,x+cw,y+ch))
                self.assertGreaterEqual(x,0)
                self.assertGreaterEqual(y,60)
                self.assertLessEqual(x+cw,w)
                self.assertLessEqual(y+ch,h)
            for i, a in enumerate(boxes):
                for b in boxes[i+1:]:
                    self.assertTrue(a[2]<=b[0] or b[2]<=a[0] or
                                    a[3]<=b[1] or b[3]<=a[1],(a,b))

    def test_edit_grid_does_not_overlap_delete_or_footer(self):
        for h in (360,380,480):
            top,bottom=139,h-103
            ch=(bottom-top-8)//2
            self.assertGreaterEqual(ch,55)
            self.assertLess(top+2*ch+8,h-96)
            self.assertLess(h-96+40,h-48+8)

if __name__ == '__main__': unittest.main()

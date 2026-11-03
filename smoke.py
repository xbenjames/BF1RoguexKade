# Windows runner check: open the window, screenshot it, close it.
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mm_gui
from PIL import ImageGrab
app = mm_gui.App()
def shot():
    app.update()
    x, y = app.winfo_rootx(), app.winfo_rooty()
    ImageGrab.grab((x, y, x + app.winfo_width(), y + app.winfo_height())).save("dist/screenshot.png")
    app.destroy()
app.after(3000, shot)
app.mainloop()
print("gui ok")

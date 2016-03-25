#/usr/bin/env python

#Import Modules
import os, pygame, copy
import math, random
from pygame.locals import *
import threading

from engine import *
import engine

if not pygame.font : print "Warning, pygame 'font' module disabled!"
if not pygame.mixer: print "Warning, pygame 'sound' module disabled!"

# --------------------------------------------------------
# Entry point
# --------------------------------------------------------
def main():
    global GAME
    # Initialize
    pygame.init()
    GAME = GameClass( "Ratastory", (1920,1080) )
    #pygame.mouse.set_visible(0)

    # Main Loop
    finished = False
    ponde, eightbit = GAME.loadFont("ponde___.ttf",16), GAME.loadFont("8bitoperator_jve.ttf",16)
    po, eb = ponde.render( "Rata Story", False, (255,255,255) ), eightbit.render( "Rata Story", False, (255,255,255) )
    por, ebr = po.get_rect(), eb.get_rect()
    por.topleft, ebr.topleft = (10,100), (10,140)
    while not finished:
        # -- CLOCK
        GAME.clock.tick(60)
        dt = GAME.clock.get_time()/1000.0
        
        # -- INPUT
        for event in pygame.event.get():
            if event.type == QUIT:
                finished = True
                break
            if event.type == KEYDOWN and event.key == K_F4:
                GAME.toggle_fullscreen()
        GAME.KEYPRESSED = pygame.key.get_pressed()
        finished = finished or GAME.KEYPRESSED[K_ESCAPE]        
        
        GAME.draw( (10,po,por) )
        GAME.draw( (10,eb,ebr) )
        # -- UPDATE
        GAME.update(dt)

    GAME.destroy()
    pygame.quit()

# Game when this script is executed, not imported
if __name__ == '__main__':
    main()

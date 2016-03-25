#/usr/bin/env python

#Import Modules
import os, pygame, copy, time
import math, random
from pygame.locals import *
import threading
# --------------------------------------------------------
# Clamping a number between min m and max M
# --------------------------------------------------------
def clamp(x,m,M):
    if   x < m : return m
    elif x > M : return M
    return x
    
GAME = None # Global GAME variable

# --------------------------------------------------------
# This thread in charge of rendering to pygame display
# --------------------------------------------------------
class DrawingThread(threading.Thread):    
    def __init__(self,game):        
        threading.Thread.__init__(self)
        self.game = game
        self.drawingbuff = []
        self.ended = False
        self.paused = False
        self.clock = pygame.time.Clock()
        self.SURFACE = pygame.Surface( (640,480), pygame.HWSURFACE)
                
    def run(self):
        while not self.ended:
            self.clock.tick(60)
            if not self.paused:
                self.SURFACE.fill((40,20,20))
                for a in self.drawingbuff:
                    self.SURFACE.blit( a[1], a[2] ) #self.game.SCREEN.blit( a[1], a[2] )
                #pygame.transform.scale(self.SURFACE, (640,480), self.game.SCREEN)
                self.game.SCREEN.blit( self.SURFACE, (0,0) )
                pygame.display.flip() # pygame flip
            else:
                time.sleep(0.25)
    
    def flip(self,buff):
        self.drawingbuff = buff
        
# --------------------------------------------------------
# Main Game class
# --------------------------------------------------------
class GameClass:
    def __init__(self,name,resolution):
        self.clock = pygame.time.Clock()
        os.environ['SDL_VIDEO_WINDOW_POS'] = "0,0"
        self.SCREENRECT= Rect(0, 0, resolution[0], resolution[1])
        self.IMAGECACHE, self.SOUNDCACHE, self.FONTCACHE = {}, {}, {}
        self.KEYPRESSED = None
        self.fullscreen = False
        bestdepth = pygame.display.mode_ok(self.SCREENRECT.size, pygame.DOUBLEBUF | pygame.HWSURFACE | pygame.NOFRAME, 32)
        self.SCREEN = pygame.display.set_mode(self.SCREENRECT.size, pygame.DOUBLEBUF | pygame.HWSURFACE | pygame.NOFRAME, bestdepth)
        self.name = name
        pygame.display.set_caption(name)
        self.newactors = []
        self.actors = []
        self.drawingbuff, self.commandbuff = [], []
        self.atfps, self.nextSound = 0.0, 0.0
        self.drawingThread = DrawingThread(self)
        self.drawingThread.start()
        #preloading
        self.loadSound("click")
        self.loadSound("xp")
        self.loadSound("bell")
        
    '''def toggle_fullscreen(self):
        self.drawingThread.paused = True        
        screen = self.SCREEN
        tmp = screen.convert()
        caption = pygame.display.get_caption()
        cursor = pygame.mouse.get_cursor()  # Duoas 16-04-2007     
        w,h = screen.get_width(),screen.get_height()
        flags = screen.get_flags()
        bits = screen.get_bitsize()    
        pygame.display.quit()
        pygame.display.init()    
        flags = flags^pygame.FULLSCREEN
        self.SCREEN = pygame.display.set_mode((w,h),flags,bits)
        self.SCREEN.blit(tmp,(0,0))
        pygame.display.set_caption(*caption) 
        pygame.key.set_mods(0) #HACK: work-a-round for a SDL bug?? 
        pygame.mouse.set_cursor( *cursor )  # Duoas 16-04-2007    
        self.drawingThread.paused = False
        '''
    '''def toggle_fullscreen(self):
        self.drawingThread.paused = True        
        newflags = screen.get_flags() ^ pygame.FULLSCREEN
        bits = screen.get_bitsize()        
        if self.fullscreen:

        else:

        self.drawingThread.paused = False
        '''



    def loadFont(self,fontname,size):
        if not pygame.font: return None
        key = (fontname,size)
        font = None
        if not self.FONTCACHE.has_key(key):
            path = "data/fonts/"+fontname
            font = pygame.font.Font(path, size)
            if font: self.FONTCACHE[key] = font
        else:
            font = self.FONTCACHE[ key ]
        return font
        
    def loadSound(self,name):
        fullname = "data/"+name #os.path.join('data', name)
        sound = None
        if not self.SOUNDCACHE.has_key(name):            
            try: 
                sound = pygame.mixer.Sound(fullname+".wav")
            except pygame.error, message:
                print 'Cannot load sound:', name
            if sound:
                self.SOUNDCACHE[name] = sound
        else:
            sound = self.SOUNDCACHE[name]
        return sound
    
    def loadImage(self,file, rotation = 0, flipx = False, flipy = False):
        key = (file, rotation, flipx, flipy)
        if not self.IMAGECACHE.has_key(key):
            path = "data/"+file #os.path.join('data', file)
            ext = ["", ".bmp", ".gif", ".png"]
            for e in ext:
                if os.path.exists(path + e):
                    path = path + e
                    break
            if rotation or flipx or flipy:
                img = self.loadImage(file)
            else:
                img = pygame.image.load(path).convert_alpha()
            if rotation:
                img = pygame.transform.rotate(img, rotation)
            if flipx or flipy:
                img = pygame.transform.flip(img, flipx, flipy)
            self.IMAGECACHE[key] = img
        return self.IMAGECACHE[key]
        
    def playSound(self,name,vol=1.0):
        if self.nextSound <= 0.0: # avoiding two very consecutive sounds
            sound = self.loadSound(name)
            sound.set_volume(vol)
            sound.play()
            self.nextSound = 0.1
        
    def destroy(self):
        self.drawingThread.ended = True
        self.drawingThread.join()
        
    def sendMessage(self,msg):
        for a in self.actors:
            a.sendMessage(msg)
        for a in self.newactors:
            a.sendMessage(msg)
            
    def addActor(self,a):
        self.newactors.append(a)
        
    def draw(self,cmd):
        self.commandbuff.append(cmd)       
    
    def update(self,dt):
        # Update fps stats
        self.atfps += dt
        self.nextSound -= dt
        if self.atfps > 3.0:
            pygame.display.set_caption(self.name + " fps: " + str(int(self.clock.get_fps())) + \
                                " / " + str(int(self.drawingThread.clock.get_fps())))
            self.atfps = 0.0
        
        # Processing actors, and we remove those terminated
        for a in self.actors:
            if a.terminated:
                self.actors.remove(a)
            else:
                a.update(dt)
        
        # Adding new actors from incoming actors buffer
        if len(self.newactors)>0:
            self.actors += self.newactors
            self.newactors = []
        
        # Swapping buffers and notifying to rendering thread the new rendering commands
        self.drawingbuff, self.commandbuff = self.commandbuff, self.drawingbuff        
        self.drawingbuff.sort()
        self.drawingThread.flip( self.drawingbuff )
        self.commandbuff = []        

# --------------------------------------------------------
# Main Entity class (contains behaviors)
# --------------------------------------------------------
class Actor:    
    def __init__(self):
        self.terminated = False
        self.behaviors = []
        
    def addBehavior(self,beh):
        self.behaviors.insert( 0, beh )
        
    def sendMessage(self,msg):
        for b in self.behaviors:
            t = hasattr(b,"terminated") and b.terminated
            if hasattr(b,"message") and not t:
                b.message(msg)
            
    def update(self,dt):
        for b in self.behaviors:            
            if hasattr(b,"terminated") and b.terminated:
                self.behaviors.remove( b )
            elif hasattr(b,"update"): 
                b.update(dt)

# --------------------------------------------------------
# Draw a sprite. Actor acts like a sprite then.
# --------------------------------------------------------
class BhDrawing:
    def __init__(self,actor,img=None,pos=(),zord=0):
        self.actor = actor
        self.actor.zord = zord
        self.actor.visible = True
        self.actor.image = None
        self.actor.rect = None
        self.actor.x,self.actor.y = 0,0
        self.actor.imageName = ""
        if img != None:
            self.actor.imageName = img
            self.actor.image = GAME.loadImage(img)
            self.actor.rect = self.actor.image.get_rect()
            self.actor.x,self.actor.y = self.actor.rect.left, self.actor.rect.top
        if pos: 
            self.actor.x, self.actor.y = pos[0],pos[1]
            
    def update(self,dt):
        if self.actor.image != None and self.actor.visible:
            self.actor.rect.topleft = (self.actor.x, self.actor.y)
            GAME.draw( ( self.actor.zord, self.actor.image, self.actor.rect ) )
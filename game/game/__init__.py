import d2d;

# example inherited player
class TestPlayer(d2d.Entity):
    def __init__(self):
        d2d.Entity.__init__(self)
        self.pos = d2d.TransformComponent()
        self.camera = d2d.CameraComponent(self.pos, True)
        self.sprite = d2d.SpriteComponent(self.pos, d2d.Filename.gameFile("textures/test32.png"), 4)
        collisionRect = d2d.FRect()
        collisionRect.x = 0
        collisionRect.y = 16
        collisionRect.w = 32
        collisionRect.h = 16
        self.collision = d2d.CollisionComponent(self.pos,collisionRect)
        self.physics = d2d.SimplePhysicsComponent(self.pos, self.collision)
        self.addComponent(self.pos)
        self.addComponent(self.camera)
        self.addComponent(self.sprite)
        self.addComponent(self.physics)
        self.addComponent(d2d.InputComponent("up", self.movement))
        self.addComponent(d2d.InputComponent("down", self.movement))
        self.addComponent(d2d.InputComponent("left", self.movement))
        self.addComponent(d2d.InputComponent("right", self.movement))
        self.addComponent(d2d.InputComponent("somemouse", self.movement))
        d2d.entityManager.addEntity(self)
        self.speed = 100
    def movement(self, eventName, param):
        print(eventName + " - " + str(param))
        x = 0
        if param == 1:
            x = self.speed
        if eventName == "up":
            self.physics.get().velocity.y = -x
        if eventName == "down":
            self.physics.get().velocity.y = x
        if eventName == "left":
            self.physics.get().velocity.x = -x
        if eventName == "right":
            self.physics.get().velocity.x = x
        

# possible way to load a map
def run():
    e = d2d.Entity()
    pos = d2d.TransformComponent()
    map = d2d.TilemapComponent(pos, d2d.Filename.gameFile("maps/test.tmx"))
    e.addComponent(pos)
    e.addComponent(map)
    d2d.entityManager.addEntity(e)



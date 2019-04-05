import d2d;

# example inherited player
class TestPlayer(d2d.Entity):
    def __init__(self):
        d2d.Entity.__init__(self)
        pos = d2d.TransformComponent()
        camera = d2d.CameraComponent(pos, True)
        self.physics = d2d.SimplePhysicsComponent(pos)
        self.addComponent(pos)
        self.addComponent(camera)
        self.addComponent(d2d.InputComponent("up", self.movement))
        self.addComponent(d2d.InputComponent("down", self.movement))
        self.addComponent(d2d.InputComponent("left", self.movement))
        self.addComponent(d2d.InputComponent("right", self.movement))
        self.addComponent(d2d.InputComponent("somemouse", self.movement))
        d2d.entityManager.addEntity(self)
    def movement(self, eventName, param):
        x = 0
        if param == 1:
            x = 50
        if eventName == "up":
            self.physics.get().velocity.y = -x
        if eventName == "down":
            self.physics.get().velocity.y = x
        if eventName == "left":
            self.physics.get().velocity.x = -x
        if eventName == "right":
            self.physics.get().velocity.x = x


# possible way to load a map
e = d2d.Entity()
pos = d2d.TransformComponent()
map = d2d.TilemapComponent(pos, d2d.Filename.gameFile("maps/test.tmx"))
e.addComponent(pos)
e.addComponent(map)
d2d.entityManager.addEntity(e)

# instance the player
player = TestPlayer()


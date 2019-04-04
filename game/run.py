import d2d;

class TestSprite(d2d.Entity):
    def __init__(self, s):
        d2d.Entity.__init__(self)
        pos = d2d.TransformComponent()
        sprite = d2d.SpriteComponent(pos, d2d.Filename.gameFile(s), 4)
        self.addComponent(pos)
        self.addComponent(sprite)
        d2d.entityManager.addEntity(self)

class TestCamera(d2d.Entity):
    def __init__(self):
        d2d.Entity.__init__(self)
        pos = d2d.TransformComponent()
        camera = d2d.CameraComponent(pos, True)
        self.addComponent(pos)
        self.addComponent(camera)
        d2d.entityManager.addEntity(self)


class EventTester(d2d.Entity):
    def __init__(self):
        d2d.Entity.__init__(self)
        inp = d2d.InputComponent("keydown", self.callback)
        inp2 = d2d.InputComponent("keyup", self.callback)
        self.addComponent(inp)
        self.addComponent(inp2)
        d2d.entityManager.addEntity(self)
    def callback(self, name, value):
        print("Got event " + str(name) + " of value " + str(value))

e = d2d.Entity()
pos = d2d.TransformComponent()
#pos.get().position = d2d.glm.vec2(200,200)
map = d2d.TilemapComponent(pos, d2d.Filename.gameFile("maps/test.tmx"))
e.addComponent(pos)
e.addComponent(map)
d2d.entityManager.addEntity(e)
TestCamera()
EventTester()

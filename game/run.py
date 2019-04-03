import d2d;

class TestSprite(d2d.Entity):
    def __init__(self, s):
        d2d.Entity.__init__(self)
        pos = d2d.TransformComponent()
        sprite = d2d.SpriteComponent(pos, d2d.Filename.gameFile(s), 4)
        self.addComponent(pos)
        self.addComponent(sprite)
        d2d.entityManager.addEntity(self)

e = d2d.Entity()
pos = d2d.TransformComponent()
pos.get().position = d2d.glm.vec2(200,200)
map = d2d.TilemapComponent(pos, d2d.Filename.gameFile("maps/test.tmx"))
e.addComponent(pos)
e.addComponent(map)
d2d.entityManager.addEntity(e)


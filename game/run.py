import d2d;

def foo():
    e = d2d.Entity()
    pos = d2d.TransformComponent()
    pos.get().scale = d2d.glm.vec2(0.1)
    sprite = d2d.SpriteComponent(pos,d2d.Filename.gameFile("textures/hihu_happy.png"), 0)
    e.addComponent(pos)
    e.addComponent(sprite)
    d2d.entityManager.addEntity(e)
foo()

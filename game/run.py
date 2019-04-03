import d2d;

def foo():
    e = d2d.Entity()
    pos = d2d.TransformComponent()
    pos.get().position = d2d.glm.vec2(200,200)
    map = d2d.TilemapComponent(pos, d2d.Filename.gameFile("maps/test.tmx"))
    e.addComponent(pos)
    e.addComponent(map)
    d2d.entityManager.addEntity(e)
foo()

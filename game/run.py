import d2d;

def foo():
    print("a")
    e = d2d.Entity()
    print("b")
    e.randomNameToDemonstrate = d2d.PositionComponent(10,10)
    e.addComponent(e.randomNameToDemonstrate)
    print("c")
    d2d.entityManager.addEntity(e)
    print("d")

foo()
foo()
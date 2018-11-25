#ifndef PHYSBODY_H
#define PHYSBODY_H

// PHYSICS WORLD
class PhysWorld {
    class NewtonWorld *m_world;

  public:
    PhysWorld();

    ~PhysWorld();
    inline const NewtonWorld *get() const { return m_world; }
    void update(const double dt);
};

// COLLISION SHAPE
class CollisionShape {
  protected:
    class NewtonCollision *m_collision;

  public:
    CollisionShape(const PhysWorld &world);
    ~CollisionShape();
    inline const NewtonCollision *get() { return m_collision; }
};

class CollisionSphere : public CollisionShape {
  public:
    CollisionSphere(const PhysWorld &world, double radius = 1.0);
};

class CollisionCuboid : public CollisionShape {
  public:
    CollisionCuboid(const PhysWorld &world, double w, double l, double h);
};

// PHYSICS BODY
class PhysBody {
  public:
    // zero mass by default (body is static)
    PhysBody(){};
    PhysBody(const PhysWorld &world, CollisionShape &&shape, const double m = 0.0, const double Ixx = 0.0, const double Iyy = 0.0, const double Izz = 0.0);
    PhysBody(const PhysBody &other);
    PhysBody(PhysBody &&other) noexcept;

    void setVelocity(const double *velocity);
    void setOmega(const double *omega);
    void getMatrix(double *mat) const;

    struct Data {
        double force[3]{0};
        double torque[3]{0};
    } data;

    ~PhysBody();

  protected:
    class NewtonBody *m_body = nullptr;
};

#endif // PHYSBODY_H
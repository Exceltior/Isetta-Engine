/*
 * Copyright (c) 2018 Isetta
 */
#include "Collisions/BoxCollider.h"
#include "Collisions/CapsuleCollider.h"
#include "Collisions/CollisionsModule.h"
#include "Collisions/SphereCollider.h"

#include "Collisions/Ray.h"
#include "Core/Debug/DebugDraw.h"
#include "Core/Math/Matrix4.h"
#include "Scene/Transform.h"

namespace Isetta {
void BoxCollider::Update() {
  DebugDraw::WireCube(
      Math::Matrix4::Translate(GetTransform()->GetWorldPos() + center) *
          Math::Matrix4::Scale(
              Math::Vector3::Scale(GetTransform()->GetWorldScale(), size)) *
          (Math::Matrix4)GetTransform()->GetWorldRot(),
      debugColor);

  DebugDraw::WireCube(Math::Matrix4::Translate(GetAABB().GetCenter()) *
                              Math::Matrix4::Scale({GetAABB().GetSize()}),
                          debugColor);
}

AABB BoxCollider::GetAABB() {
  AABB aabb{GetTransform()->GetWorldPos() + center, Math::Vector3::zero};
  
  auto transform = GetTransform();
  Math::Vector3 point1, point2, point3, point4, point5, point6, point7, point8;
  point1 = transform->WorldPosFromLocalPos(center - size / 2);
  point2 = transform->WorldPosFromLocalPos(center - size / 2 +
                                           Math::Vector3::right * size.x);
  point3 = transform->WorldPosFromLocalPos(center - size / 2 +
                                           Math::Vector3::up * size.y);
  point4 = transform->WorldPosFromLocalPos(center - size / 2 +
                                           Math::Vector3::up * size.y +
                                           Math::Vector3::right * size.x);

  point5 = transform->WorldPosFromLocalPos(center - size / 2 +
                                           Math::Vector3::back * size.z);
  point6 = transform->WorldPosFromLocalPos(center - size / 2 +
                                           Math::Vector3::right * size.x +
                                           Math::Vector3::back * size.z);
  point7 = transform->WorldPosFromLocalPos(center - size / 2 +
                                           Math::Vector3::up * size.y +
                                           Math::Vector3::back * size.z);
  point8 = transform->WorldPosFromLocalPos(
      center - size / 2 + Math::Vector3::up * size.y +
      Math::Vector3::back * size.z + Math::Vector3::right * size.x);
  aabb.Encapsulate(point1);
  aabb.Encapsulate(point2);
  aabb.Encapsulate(point3);
  aabb.Encapsulate(point4);
  aabb.Encapsulate(point5);
  aabb.Encapsulate(point6);
  aabb.Encapsulate(point7);
  aabb.Encapsulate(point8);
  return aabb;
}

bool BoxCollider::Raycast(const Ray& ray, RaycastHit* const hitInfo,
                          float maxDistance) {
  float tmin = -INFINITY, tmax = maxDistance > 0 ? maxDistance : INFINITY;
  Math::Vector3 e = GetWorldExtents();
  Math::Vector3 o = GetTransform()->LocalPosFromWorldPos(ray.GetOrigin());
  Math::Vector3 d = GetTransform()->LocalDirFromWorldDir(ray.GetDirection());
  Math::Vector3 invD = 1.0f / d;

  float t[6];
  for (int i = 0; i < 3; i++) {
    t[2 * i] = -(e[i] + o[i]) * invD[i];
    t[2 * i + 1] = (e[i] - o[i]) * invD[i];
  }
  tmin =
      Math::Util::Max({Math::Util::Min(t[0], t[1]), Math::Util::Min(t[2], t[3]),
                       Math::Util::Min(t[4], t[5])});
  tmax =
      Math::Util::Min({Math::Util::Max(t[0], t[1]), Math::Util::Max(t[2], t[3]),
                       Math::Util::Max(t[4], t[5])});
  if (tmax < 0 || tmin > tmax) return false;
  if (tmin < 0) tmin = tmax;

  Math::Vector3 n;  // TODO (normal)
  RaycastHitCtor(hitInfo, tmin,
                 GetTransform()->WorldPosFromLocalPos(o) +
                     GetTransform()->WorldDirFromLocalDir(d) * tmin,
                 n);
  return true;
}

INTERSECTION_TEST(BoxCollider)
}  // namespace Isetta

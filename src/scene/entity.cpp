#include "star/scene/entity.hpp"

namespace star {

bool Entity::operator==(EntityId uint32) const {
    return _id == uint32;
}

}

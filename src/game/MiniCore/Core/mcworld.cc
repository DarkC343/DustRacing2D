// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mcworld.hh"
#include "mcworldimpl.hh"
#include "mcforcegenerator.hh"
#include "mcfrictiongenerator.hh"
#include "mcobject.hh"
#include "mcobjecttree.hh"
#include "mccamera.hh"

#include <cassert>

MCWorld * MCWorldImpl::pInstance = nullptr;

MCWorldImpl::MCWorldImpl()
: pObjectTree(nullptr)
, minX(0)
, maxX(0)
, minY(0)
, maxY(0)
, minZ(0)
, maxZ(0)
{}

MCWorldImpl::~MCWorldImpl()
{}

void MCWorldImpl::integrate(MCFloat step)
{
    // Integrate and update all registered objects
    forceRegistry.update();
    for (MCUint i = 0; i < objs.size(); i++) {
        MCObject * const object(objs[i]);
        if (object->physicsObject() && !object->stationary()) {
            object->integrate(step);
            object->stepTime();
        } else {
            object->stepTime();
        }
    }
}

void MCWorldImpl::detectCollisions()
{
    // Check collisions for all registered objects
    static MCObjectTree::ObjectSet collisions;
    for (MCUint i = 0; i < objs.size(); i++) {
        MCObject * const object(objs[i]);
        if (object->physicsObject()) {
            collisions.clear();
            pObjectTree->getBBoxCollisions(*object, collisions);
            auto j1 = collisions.begin();
            auto j2 = collisions.end();
            while (j1 != j2) {
                contactResolver.processPossibleCollision(*object, **j1);
                j1++;
            }
        }
    }
}

MCContact * MCWorldImpl::getDeepestInterpenetration(
    const std::vector<MCContact *> & contacts)
{
    MCFloat maxDepth = 0;
    MCContact * bestContact = nullptr;
    for (MCContact * contact : contacts) {
        if (contact->interpenetrationDepth() > maxDepth) {
            maxDepth = contact->interpenetrationDepth();
            bestContact = contact;
        }
    }
    return bestContact;
}

void MCWorldImpl::processContacts(MCObject & object)
{
    MCContact * deepestContact = nullptr;
    auto iter(object.contacts().begin());
    iter = object.contacts().begin();
    for (; iter != object.contacts().end(); iter++) {
        deepestContact = getDeepestInterpenetration(iter->second);
        if (deepestContact) {
            processContact(object, *deepestContact);
        }
    }
    object.deleteContacts();
}

void MCWorldImpl::processContacts()
{
    for (MCObject * object : objs) {
        if (object->physicsObject()) {
            processContacts(*object);
        }
    }
}

void MCWorldImpl::processContact(MCObject & object, MCContact & contact)
{
    MCObject & pa(object);
    MCObject & pb(contact.object());

    const MCFloat restitution(
        std::min(pa.restitution(), pb.restitution()));
    const MCVector2d<MCFloat> diff(pb.velocity() - pa.velocity());
    const MCVector3d<MCFloat> impulse(
        contact.contactNormal() * contact.contactNormal().dot(diff));
    const MCVector3d<MCFloat> displacement(
        contact.contactNormal() * contact.interpenetrationDepth());

    const MCFloat a = pa.invMass();
    const MCFloat b = pb.invMass();

    if (!pa.stationary()) {
        // Move colliding object
        const MCFloat s = a / (a + b);
        pa.displace(displacement * s);

        // Apply impulse
        pa.addImpulse((impulse + impulse * restitution) * s);
    }

    if (!pb.stationary()) {
        // Move colliding object
        const MCFloat s = b / (a + b);
        pb.displace(-displacement * s);

        // Apply impulse
        pb.addImpulse((-impulse - impulse * restitution) * s);
    }

    // Remove contacts with pa from pb, because physically it was already handled here
    pb.deleteContacts(pa);
}

void MCWorldImpl::render(MCCamera * pCamera)
{
    // Render in the order of the layers
    LayerHash::iterator j;
    LayerHash::iterator end;
    MCObject * pObj = nullptr;
    for (MCUint i = 0; i < MCWorld::MaxLayers; i++) {
        j   = layers[i].begin();
        end = layers[i].end();
        for (; j != end; j++) {
            pObj = *j;
            if (pObj->renderable()) {
                if ((pCamera && pCamera->isVisible(pObj->bbox())) || !pCamera) {
                    pObj->render(pCamera);
                }
            }
        }
    }
}

void MCWorldImpl::renderShadows(MCCamera * pCamera)
{
    const MCUint i2 = objs.size();
    MCObject * pObj = nullptr;
    for (MCUint i = 0; i < i2; i++) {
        pObj = objs[i];
        if (pObj->renderable() && pObj->hasShadow()) {
            if ((pCamera && pCamera->isVisible(pObj->bbox())) || !pCamera) {
                pObj->renderShadow(pCamera);
            }
        }
    }
}

MCWorld::MCWorld()
: m_pImpl(new MCWorldImpl)
{
    if (!MCWorldImpl::pInstance) {
        MCWorldImpl::pInstance = this;
    } else {
        std::cerr << "ERROR!!: Only one MCWorld can exist!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

MCWorld & MCWorld::instance()
{
    if (!MCWorldImpl::pInstance) {
        std::cerr << "ERROR!!: MCWorld instance not created!" << std::endl;
        exit(EXIT_FAILURE);
    }
    return *MCWorldImpl::pInstance;
}

void MCWorld::setDimensions(
    MCFloat minX, MCFloat maxX,
    MCFloat minY, MCFloat maxY,
    MCFloat minZ, MCFloat maxZ)
{
    // Set dimensions for minimum objectTree leaves
    const MCFloat MinLeafWidth  = 64;
    const MCFloat MinLeafHeight = 64;

    // Init objectTree
    delete m_pImpl->pObjectTree;
    m_pImpl->pObjectTree = new MCObjectTree(
        minX, minY, maxX, maxY, MinLeafWidth, MinLeafHeight);

    // Set dimensions
    m_pImpl->minX = minX;
    m_pImpl->maxX = maxX;
    m_pImpl->minY = minY;
    m_pImpl->maxY = maxY;
    m_pImpl->minZ = minZ;
    m_pImpl->maxZ = maxZ;
}

MCFloat MCWorld::minX() const
{
    return m_pImpl->minX;
}

MCFloat MCWorld::maxX() const
{
    return m_pImpl->maxX;
}

MCFloat MCWorld::minY() const
{
    return m_pImpl->minY;
}

MCFloat MCWorld::maxY() const
{
    return m_pImpl->maxY;
}

MCFloat MCWorld::minZ() const
{
    return m_pImpl->minZ;
}

MCFloat MCWorld::maxZ() const
{
    return m_pImpl->maxZ;
}

void MCWorld::addObject(MCObject & object)
{
    m_pImpl->addObject(object);
}

void MCWorldImpl::addObject(MCObject & object)
{
    if (!object.removing()) {
        if (object.index() == -1) {

            // Add to layer map
            addToLayerMap(object);

            // Add to object vector (O(1))
            objs.push_back(&object);
            object.setIndex(objs.size() - 1);

            // Add to ObjectTree
            if (object.physicsObject() && !object.bypassCollisions()) {
                pObjectTree->insert(object);
            }

            // Add xy friction
            const MCFloat FrictionThreshold = 0.001f;
            if (object.xyFriction() > FrictionThreshold) {
                forceRegistry.addForceGenerator(
                    *new MCFrictionGenerator(
                        object.xyFriction(), object.xyFriction()), object, true);
            }
        }
    } else {
        object.setRemoving(false);
    }
}

void MCWorld::removeObject(MCObject & object)
{
    object.setRemoving(true);
    m_pImpl->removeObjs.push_back(&object);
}

void MCWorld::removeObjectNow(MCObject & object)
{
    object.setRemoving(true);
    for (MCObject * obj : m_pImpl->objs) {
        if (obj != &object) {
            if (obj->physicsObject()) {
                obj->deleteContacts(object);
            }
        }
    }

    m_pImpl->removeObject(object);
}

void MCWorldImpl::removeObject(MCObject & object)
{
    // Reset motion
    object.resetMotion();

    // Remove from the layer map
    removeFromLayerMap(object);

    // Remove from object vector (O(1))
    if (object.index() > -1 && object.index() < static_cast<int>(objs.size())) {
        objs[object.index()] = objs.back();
        objs[object.index()]->setIndex(object.index());
        objs.pop_back();
        object.setIndex(-1);
    }

    // Remove from ObjectTree
    if (object.physicsObject() && !object.bypassCollisions()) {
        pObjectTree->remove(object);
    }

    object.setRemoving(false);
}

void MCWorldImpl::processRemovedObjects()
{
    for (MCObject * obj : removeObjs) {
        if (obj->removing()) {
            removeObject(*obj);
        }
    }
    removeObjs.clear();
}

void MCWorldImpl::addToLayerMap(MCObject & object)
{
    const MCUint layerIndex =
        object.layer() >= MCWorld::MaxLayers ? MCWorld::MaxLayers - 1 : object.layer();
    layers[layerIndex].insert(&object);
}

void MCWorldImpl::removeFromLayerMap(MCObject & object)
{
    const MCUint layerIndex =
        object.layer() >= MCWorld::MaxLayers ? MCWorld::MaxLayers - 1 : object.layer();
    layers[layerIndex].erase(&object);
}

void MCWorld::addToLayerMap(MCObject & object)
{
    m_pImpl->addToLayerMap(object);
}

void MCWorld::removeFromLayerMap(MCObject & object)
{
    m_pImpl->removeFromLayerMap(object);
}

void MCWorld::addForceGenerator(
    MCForceGenerator & gen, MCObject & obj, bool takeOwnership)
{
    m_pImpl->forceRegistry.addForceGenerator(gen, obj, takeOwnership);
}

void MCWorld::removeForceGenerator(MCForceGenerator & gen, MCObject & obj)
{
    m_pImpl->forceRegistry.removeForceGenerator(gen, obj);
}

void MCWorld::stepTime(MCFloat step)
{
    // Integrate physics
    m_pImpl->integrate(step);

    // Detect all collisions and generate contacts
    m_pImpl->detectCollisions();

    // Process contacts
    m_pImpl->processContacts();

    // Remove objects that are marked to be removed
    m_pImpl->processRemovedObjects();
}

void MCWorld::render(MCCamera * pCamera)
{
    m_pImpl->render(pCamera);
}

void MCWorld::renderShadows(MCCamera * pCamera)
{
    m_pImpl->renderShadows(pCamera);
}

MCWorld::ObjectVector MCWorld::objects() const
{
    return m_pImpl->objs;
}

MCObjectTree & MCWorld::objectTree() const
{
    assert(m_pImpl->pObjectTree);
    return *m_pImpl->pObjectTree;
}

MCWorld::~MCWorld()
{
    delete m_pImpl->pObjectTree;
    delete m_pImpl;
}

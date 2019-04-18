/*
    animation.cpp: animation system
    Copyright (C) 2019 Malte Kieﬂling
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "animation.h"

#include "SDL.h"
#include "tinyxml2.h"
#include "util/xmlhelpers.h"
namespace xml = tinyxml2;

void AnimatedSprite::play(const std::string& name)
{
    stop();
    for (size_t i = 0; i < animations.size(); i++) {
        if (animations[i].name == name) {
            animations[i].playing = true;
            current = i;
            break;
        }
    }
}

void AnimatedSprite::pause()
{
    if (current >= animations.size()) {
        return;
    }

    animations[current].playing = false;
}

void AnimatedSprite::resume()
{
    if (current >= animations.size()) {
        return;
    }

    animations[current].playing = true;
}

void AnimatedSprite::stop()
{
    for (auto&& animation : animations) {
        animation.currentFrame = 0;
        animation.playing = false;
        animation.frameTime = 0.0;
    }
}

void AnimatedSprite::load(const std::string& filename)
{
    animations.clear();
    xml::XMLDocument doc;
    auto err = doc.LoadFile(filename.c_str());
    if (err != xml::XML_SUCCESS) {
        SDL_Log("Cannot open animation file %s", filename.c_str());
        return;
    }
    if (auto rootTag = doc.FirstChildElement("animations"); rootTag) {
        defaultAnimation = nullAwareAttr(rootTag->Attribute("default"));
        for (
            auto animationTag = rootTag->FirstChildElement("animation");
            animationTag;
            animationTag = animationTag->NextSiblingElement("animation")) {
            Animation animation;
            animation.name = nullAwareAttr(animationTag->Attribute("name"));
            animation.loop = animationTag->BoolAttribute("loop", true);
            for (
                auto frameTag = animationTag->FirstChildElement("frame");
                frameTag;
                frameTag = frameTag->NextSiblingElement("frame")) {
                Frame frame;
                frame.duration = frameTag->DoubleAttribute("duration", 0.0);
                frame.src.x = frameTag->IntAttribute("x", 0);
                frame.src.y = frameTag->IntAttribute("y", 0);
                frame.src.w = frameTag->IntAttribute("width", 0);
                frame.src.h = frameTag->IntAttribute("height", 0);
                animation.frames.push_back(frame);
            }
            animations.push_back(animation);
        }
    }
}

void AnimatedSprite::save(const std::string& filename)
{
    xml::XMLDocument doc;
    auto rootTag = doc.NewElement("animations");
    rootTag->Attribute("default", defaultAnimation.c_str());
    for (const auto& animation : animations) {
        auto animationTag = doc.NewElement("animation");
        animationTag->SetAttribute("name", animation.name.c_str());
        animationTag->SetAttribute("loop", animation.loop);
        for (const auto& frame : animation.frames) {
            auto frameTag = doc.NewElement("frame");
            frameTag->SetAttribute("duration", frame.duration);
            frameTag->SetAttribute("x", frame.src.x);
            frameTag->SetAttribute("y", frame.src.y);
            frameTag->SetAttribute("width", frame.src.w);
            frameTag->SetAttribute("height", frame.src.h);
            animationTag->InsertEndChild(frameTag);
        }
        rootTag->InsertEndChild(animationTag);
    }

    doc.InsertFirstChild(rootTag);
    doc.SaveFile(filename.c_str());
}

std::shared_ptr<AnimationSystem> AnimationSystem::instance(nullptr);

AnimationSystem::IndexType AnimationSystem::create(const SpriteSystem::IndexType& spriteId, const std::string& filename)
{
    AnimatedSprite animation;
    animation.load(filename);
    animation.spriteId = spriteId;

    // figure out and set the default rect
    auto& sprite = SpriteSystem::instance->get(spriteId);
    size_t defaultAnimationId = 0;
    // a default animation is set
    if (!animation.defaultAnimation.empty()) {
        for (size_t i = 0; i < animation.animations.size(); i++) {
            if (animation.defaultAnimation == animation.animations[i].name) {
                defaultAnimationId = i;
                break;
            }
        }
    }
    // animation exsists, has frames (?), then we can set the rect
    if (defaultAnimationId < animation.animations.size()) {
        animation.current = defaultAnimationId; // also set that, so theoretically one could resume immediatly!
        if (!animation.animations[defaultAnimationId].frames.empty()) {
            sprite.source = animation.animations[defaultAnimationId].frames[0].src;
        }
    }

    return animations.insert(animation);
}

AnimationSystem::IndexType AnimationSystem::create(const SpriteComponent& sprite, const std::string& filename)
{
    return create(sprite.getIndex(), filename);
}

AnimatedSprite& AnimationSystem::get(const IndexType& i)
{
    static AnimatedSprite dummy;
    if (auto iter = animations.find(i); iter != animations.end()) {
        return *iter;
    }
    return dummy;
}

void AnimationSystem::remove(const IndexType& i)
{
    animations.remove(i);
}

void AnimationSystem::update(double dt)
{
    for (auto&& animation : animations) {
        if (animation.current >= animation.animations.size()) {
            continue;
        }

        auto& current = animation.animations[animation.current];
        if (!current.playing) {
            continue;
        }

        if (current.currentFrame >= current.frames.size()) {
            continue;
        }

        current.frameTime += dt;
        // check if we need to go to the next frame
        if (current.frames[current.currentFrame].duration <= current.frameTime) {
            current.frameTime = 0.0;
            ++current.currentFrame;
            // next frame is past the last one
            if (current.currentFrame >= current.frames.size()) {
                current.currentFrame = 0;
                if (!current.loop) {
                    // stop playing and iterate on
                    current.playing = false;
                    continue;
                }
            }
        }

        // update the sprite
        auto& sprite = SpriteSystem::instance->get(animation.spriteId);
        sprite.source = current.frames[current.currentFrame].src;
    }
}

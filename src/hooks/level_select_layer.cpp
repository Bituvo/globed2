#include "level_select_layer.hpp"

#include <data/packets/client/general.hpp>
#include <data/packets/server/general.hpp>
#include <net/network_manager.hpp>

using namespace geode::prelude;

bool HookedLevelSelectLayer::init(int p0) {
    if (!LevelSelectLayer::init(p0)) return false;

    auto& nm = NetworkManager::get();

    if (nm.established()) {
        nm.send(RequestPlayerCountPacket::create(std::move(std::vector(MAIN_LEVELS))));
        nm.addListener<LevelPlayerCountPacket>([this](auto packet) {
            auto currentLayer = getChildOfType<LevelSelectLayer>(CCScene::get(), 0);
            if (currentLayer && this != currentLayer) return;

            m_fields->levels.clear();
            for (const auto& level : packet->levels) {
                m_fields->levels[level.first] = level.second;
            }

            this->updatePlayerCounts();
        });
    }

    return true;
}

void HookedLevelSelectLayer::destructor() {
    LevelSelectLayer::~LevelSelectLayer();

    auto& nm = NetworkManager::get();
    nm.removeListener<LevelPlayerCountPacket>(util::time::seconds(3));
}

void HookedLevelSelectLayer::sendRequest(float) {
    auto& nm = NetworkManager::get();
    if (nm.established()) {
        nm.send(RequestPlayerCountPacket::create(std::move(std::vector(MAIN_LEVELS))));
    }
}

void HookedLevelSelectLayer::updatePlayerCounts() {
    auto* bsl = getChildOfType<BoomScrollLayer>(this, 0);
    if (!bsl) return;
    auto* extlayer = getChildOfType<ExtendedLayer>(bsl, 0);
    if (!extlayer || extlayer->getChildrenCount() == 0) return;

    for (auto* page : CCArrayExt<LevelPage*>(extlayer->getChildren())) {
        CCLabelBMFont* label = static_cast<CCLabelBMFont*>(page->getChildByID("player-count-label"_spr));

        if (page->m_level->m_levelID < 0) {
            if (label) label->setVisible(false);
            continue;
        }

        if (!label) {
            auto pos = getChildOfType<CCMenu>(page, 0)->getPosition() + CCPoint{0.f, -37.f};
            Build<CCLabelBMFont>::create("", "bigFont.fnt")
                .pos(pos)
                .scale(0.4f)
                .id("player-count-label"_spr)
                .parent(page)
                .store(label);
        } else {
            label->setVisible(true);
        }

        if (m_fields->levels.contains(page->m_level->m_levelID)) {
            label->setString(fmt::format("{} players", m_fields->levels[page->m_level->m_levelID]).c_str());
        } else {
            label->setString("? players");
        }
    }
}

void HookedLevelSelectLayer::updatePageWithObject(cocos2d::CCObject* o1, cocos2d::CCObject* o2) {
    LevelSelectLayer::updatePageWithObject(o1, o2);

    this->retain();
    Loader::get()->queueInMainThread([this] {
        this->updatePlayerCounts();
        this->release();
    });
}
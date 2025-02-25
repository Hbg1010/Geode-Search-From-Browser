#include <camila314.geode-uri/include/GeodeURI.hpp>

using namespace geode::prelude;

void addSearchLayer(SearchType type, const std::string& input) {
	if (auto pl = PlayLayer::get()) {
		if (!pl->m_isPaused || (pl->getParent() && !pl->getParent()->getChildByType<PauseLayer>(0))) UILayer::get()->onPause(nullptr);
		// bool shouldReplace = false;
		geode::createQuickPopup(
			"HOLD ON!",            // title
			"You are about to be redirected out of this level! Are you sure you want leave?",   // content
			"No", "Exit",      // buttons
			[=](auto, bool btn2) {
				if (!btn2) return;
				// log::debug("{}", input);
				PauseLayer* pauseLayer = pl->getParent()->getChildByType<PauseLayer>(0);
				if (!pauseLayer) pauseLayer = PauseLayer::create(false);
				if (!pauseLayer) return;
				pauseLayer->onQuit(nullptr);
				auto search = GJSearchObject::create(type, input);
				// log::debug("{}", static_cast<int>(searchObj->m_searchType));
				auto levelLayer = LevelBrowserLayer::scene(search);
				CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, levelLayer));
			}
		);		
	} else if (auto lel = LevelEditorLayer::get()) {
		geode::createQuickPopup(
			"HOLD ON!",            // title
			"You are about to be redirected out of the editor. Are you sure you don't want to save before exiting?",   // content
			"No", "Exit",      // buttons
			[=](auto, bool btn2) {
				if (!btn2) return;
				EditorPauseLayer* editorPause = EditorPauseLayer::create(lel);
				if (!editorPause) return;
				editorPause->onSaveAndPlay(nullptr);
				auto search = GJSearchObject::create(type, input);
				// log::debug("{}", static_cast<int>(searchObj->m_searchType));
				auto levelLayer = LevelBrowserLayer::scene(search);
				CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, levelLayer));
			}
		);
	} else {
		auto search = GJSearchObject::create(type, input);
		auto levelLayer = LevelBrowserLayer::scene(search);
		CCDirector::sharedDirector()->pushScene(levelLayer);
	}
}


$on_mod(Loaded) {
	// Level searching: TODO figure out correct sorting with links
	handleURI("level", [](std::string const& path) {
		addSearchLayer(SearchType::Search, path);
	});

	// Handles searching for players
	handleURI("user", [](std::string const& path) {
		addSearchLayer(SearchType::Users, path);
	});
};



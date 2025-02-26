#include <camila314.geode-uri/include/GeodeURI.hpp>

using namespace geode::prelude;

// used to modify return to last scene
#include <Geode/modify/GameManager.hpp>
class $modify(awesomeGM, GameManager) {

	struct Fields {
		CCScene* thing;
	};

	void returnToLastScene(GJGameLevel* p0) {
		if (std::ref(m_fields->thing) == nullptr) {
			// log::debug("{}", p0->m_localOrSaved);
			GameManager::returnToLastScene(p0);
		} else {
			const int ID = p0->m_levelID.value();
			CCScene* sceneAhead = nullptr;

			if (p0->m_localOrSaved){
				sceneAhead = EditLevelLayer::scene(p0);
			} else if (ID <= 23) {
				sceneAhead = LevelSelectLayer::scene(ID-1);
			} else if (ID == 3001) {
				sceneAhead = SecretLayer2::scene();
			} else if (ID > 5000 && ID < 5005) {
				sceneAhead = LevelAreaInnerLayer::scene(true);
			} else {
				sceneAhead = LevelInfoLayer::scene(p0, false);
			}
			
			CCDirector::sharedDirector()->replaceScene(sceneAhead);
			CCDirector::sharedDirector()->pushScene(m_fields->thing);
			m_fields->thing = nullptr;
		}
	}

	// stores a scene to the gamemanager instance
	void tempField(CCScene* sent) {
		m_fields->thing = sent;
	}
};

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
				// GameManager::get()->returnToLastScene();
				auto search = GJSearchObject::create(type, input);
				auto levelLayer = LevelBrowserLayer::scene(search);
				awesomeGM* manager = static_cast<awesomeGM*>(GameManager::get());
				manager->tempField(levelLayer);
				pauseLayer->onQuit(nullptr);
				// delayedReplace(type, input);
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
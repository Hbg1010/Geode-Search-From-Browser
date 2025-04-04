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

void playSearch(SearchType type, const std::string& input, PlayLayer* pl = PlayLayer::get(), int searchMode) {
	PauseLayer* pauseLayer = pl->getParent()->getChildByType<PauseLayer>(0);
	if (!pauseLayer) pauseLayer = PauseLayer::create(false);
	if (!pauseLayer) return;

	auto search = GJSearchObject::create(type, input);
	auto levelLayer = LevelBrowserLayer::scene(search);
	awesomeGM* manager = static_cast<awesomeGM*>(GameManager::get());
	manager->tempField(levelLayer);
	pauseLayer->onQuit(nullptr);
}

void editorSearch(SearchType type, const std::string& input, LevelEditorLayer* lel = LevelEditorLayer::get(), int searchMode) {
	EditorPauseLayer* editorPause = EditorPauseLayer::create(lel);
	if (!editorPause) return;

	auto search = GJSearchObject::create(type, input);

	if (Mod::get()->getSettingValue<bool>("saveByDefault")) {
		editorPause->onSaveAndPlay(nullptr);
	} else {
		editorPause->onExitNoSave(nullptr);
	}

	auto levelLayer = LevelBrowserLayer::scene(search);
	CCDirector::sharedDirector()->pushScene(levelLayer);
	// CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, levelLayer));
}


void addSearchLayer(SearchType type, const std::string& input, int searchMode = 0) {
	if (auto pl = PlayLayer::get()) {
		if (!pl->m_isPaused || (pl->getParent() && !pl->getParent()->getChildByType<PauseLayer>(0))) UILayer::get()->onPause(nullptr);
			if (Mod::get()->getSettingValue<bool>("plPopup")) {
				geode::createQuickPopup(
					"HOLD ON!",            // title
					"You are about to be redirected out of this level! Are you sure you want leave?",   // content
					"No", "Exit",      // buttons
					[=](auto, bool btn2) {
						if (btn2) playSearch(type, input, pl, searchMode);
				});		
			} else playSearch(type, input, pl, searchMode);

	} else if (auto lel = LevelEditorLayer::get()) {
		if (Mod::get()->getSettingValue<bool>("editorPopup")) {
			geode::createQuickPopup(
				"HOLD ON!",            // title
				"You are about to be redirected out of the editor. Your changes will be saved!",   // content
				"No", "Exit",      // buttons
				[=](auto, bool btn2) {
					if (btn2) return editorSearch(type, input, lel, searchMode);
				}
			);
		} else editorSearch(type, input, lel, searchMode);

	} else {
		auto search = GJSearchObject::create(type, input);
		search->m_searchMode = searchMode;
		auto levelLayer = LevelBrowserLayer::scene(search);
		CCDirector::sharedDirector()->pushScene(levelLayer);
	}
}

#include <Geode/modify/LevelBrowserLayer.hpp>
class $modify(ieatfarts, LevelBrowserLayer) {
	bool init(GJSearchObject* searchBy) {
		if (!LevelBrowserLayer::init(searchBy)) return false;

		log::debug("{}", static_cast<int>(searchBy->m_searchType));
		log::debug("mode {}", static_cast<int>(searchBy->m_searchMode));

		return true;
	}
};

$on_mod(Loaded) {
	// Level searching: TODO figure out correct sorting with links
	handleURI("level", [](std::string const& path) {
		addSearchLayer(SearchType::Search, path);
	});

	// Handles searching for players
	handleURI("user", [](std::string const& path) {
		addSearchLayer(SearchType::Users, path);
	});

	handleURI("list", [](std::string const& path) {
		addSearchLayer(SearchType::Search, path, 1); // 1 is list mode
	});
};
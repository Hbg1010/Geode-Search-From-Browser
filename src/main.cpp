#include <camila314.geode-uri/include/GeodeURI.hpp>
#include <vector>
using namespace geode::prelude;

// unused for now!!
// std::vector<std::string> splitString(const std::string& str, char delimiter) {
//     std::vector<std::string> tokens;
//     std::stringstream ss(str);
//     std::string token;
//     while (std::getline(ss, token, delimiter)) {
//         tokens.push_back(token);
//     }
//     return tokens;
// }

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

void playSearch(SearchType type, const std::string& input, int searchMode, PlayLayer* pl = PlayLayer::get()) {
	PauseLayer* pauseLayer = pl->getParent()->getChildByType<PauseLayer>(0);
	if (!pauseLayer) pauseLayer = PauseLayer::create(false);
	if (!pauseLayer) return;

	auto search = GJSearchObject::create(type, input);
	search->m_searchMode = searchMode;
	auto levelLayer = LevelBrowserLayer::scene(search);
	awesomeGM* manager = static_cast<awesomeGM*>(GameManager::get());
	manager->tempField(levelLayer);
	pauseLayer->onQuit(nullptr);
}

void editorSearch(SearchType type, const std::string& input, int searchMode, LevelEditorLayer* lel = LevelEditorLayer::get(), bool save = Mod::get()->getSettingValue<bool>("saveByDefault")) {
	EditorPauseLayer* editorPause = EditorPauseLayer::create(lel);
	if (!editorPause) return;

	auto search = GJSearchObject::create(type, input);
	search->m_searchMode = searchMode;

	if (save) {
		editorPause->onSaveAndPlay(nullptr);
	} else {
		editorPause->onExitNoSave(nullptr);
	}

	auto levelLayer = LevelBrowserLayer::scene(search);
	// CCDirector::sharedDirector()->pushScene(levelLayer);
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, levelLayer));
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
						if (btn2) playSearch(type, input, searchMode, pl);
				});		
			} else playSearch(type, input, searchMode, pl);

	} else if (auto lel = LevelEditorLayer::get()) {
		if (Mod::get()->getSettingValue<bool>("editorPopup")) {

			std::string description = "You are about to be redirected out of the editor.";

			if (!Mod::get()->getSettingValue<bool>("savePopup")) {
				if (Mod::get()->getSettingValue<bool>("saveByDefault")) {
					description = description + " Your changes will be saved!";
				} else {
					description = description +  "Your changes <cr>won't</c> be saved!";
				}
			}

			// creates the popup
			geode::createQuickPopup(
				"HOLD ON!",            // title
				description,
				"No", "Exit",      // buttons
				[=](auto, bool btn2) {
					if (!btn2) return;
					if (!Mod::get()->getSettingValue<bool>("savePopup")) return editorSearch(type, input, searchMode, lel);

					geode::createQuickPopup(
						"Save?",            // title
						"Do you want to save",   // content
						"Exit", "Save",      // buttons
						[=](auto, bool button2) {
							editorSearch(type, input, searchMode, lel, button2);
						}
					);
				}
			);
		} else editorSearch(type, input, searchMode, lel);

	} else {
		auto search = GJSearchObject::create(type, input);
		search->m_searchMode = searchMode;
		auto levelLayer = LevelBrowserLayer::scene(search);
		CCDirector::sharedDirector()->pushScene(levelLayer);
	}
}

// these are the handles that you are able to use 
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

	handleURI("mappacks", [](std::string const& path) {
		addSearchLayer(SearchType::MapPack, path);
	});

	// TODO

	// handleURI("mappack", [](std::string const& path) {
	// 	addSearchLayer(SearchType::MapPack, path); // 1 is list mode
	// });

	/*
	This is a pain in the ass and is NOT a priority. sorry gang.
	*/

	// handleURI("levelfilter", [](std::string const& path) {
	// 	// std::vector resault = splitString(path, '/');

	// 	SearchType searchBy;

	// 	switch(path) {
	// 		case "search":
	// 		case "0":
	// 			searchBy = SearchType::Search;
	// 			break;
	// 		case "downloaded":
	// 		case "downloads":
	// 		case "1":
	// 			searchBy = SearchType::Downloaded;
	// 			break;
	// 		case "likes":
	// 		case "2":
	// 			searchBy = SearchType::MostLiked;
	// 			break;
	// 		case "trending":
	// 		case "3":
	// 			searchBy = SearchType::Trending;
	// 			break;
	// 		case "recent":
	// 		case "4":
	// 			searchBy = SearchType::Recent;
	// 			break;
	// 		case "featured":
	// 		case "6":
	// 			searchBy = SearchType::Featured;
	// 			break;
	// 		case "magic":
	// 		case "7":
	// 			searchBy = SearchType::Magic;
	// 			break;
	// 		case "sends":
	// 		case "8":
	// 			searchBy = SearchType::Sends;
	// 			break;
	// 		case "awarded":
	// 		case "11":
	// 			searchBy = SearchType::Awarded;
	// 			break;
	// 		case "followed":
	// 		case "12":
	// 			searchBy = SearchType::Followed;
	// 			break;
	// 		case "friends":
	// 		case "13":
	// 			searchBy = SearchType::Friends;
	// 			break;
	// 		case "friends":
	// 		case "14":
	// 			searchBy = SearchType::Friends;
	// 			break;
	// 		default:
	// 			Notification::create("Invalid search filter", NotificationIcon::Error, 3)->show();
	// 			return;
	// 	}

	// 	addSearchLayer(searchBy, path);
	// });
};
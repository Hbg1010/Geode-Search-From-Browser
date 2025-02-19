/**
 * Small mod using Camila314's Geode-URI library that allows level searching from browser input
 * 
 * To access levels ingame: 
 * Geode://level/<ID>
 * 
 * To access players: Geode://user/Name
 * 
 * TODO: fix touch priority, or fix the way the scene stack works 
 */

#include <camila314.geode-uri/include/GeodeURI.hpp>
#include <Geode/Geode.hpp> 

using namespace geode::prelude;

class LevelBrowserLayerFromSearch : public LevelBrowserLayer {
	void keyBackClicked() override {
		if (true) {
			CCDirector::sharedDirector()->popScene();
		} else {
			LevelBrowserLayer::keyBackClicked();
		}
	}
};

$on_mod(Loaded) {
	handleURI("alert", [](std::string const& path) {
		FLAlertLayer::create("Custom Alert", path, "Ok")->show();
	});

	// Level searching: TODO figure out correct sorting with links
	handleURI("level", [](std::string const& path) {
		GJSearchObject* x = GJSearchObject::create(SearchType::Search, path);
		auto levelLayer = LevelBrowserLayer::scene(x);

		if (auto x = typeinfo_cast<PlayLayer*>(CCScene::get()->getChildren()->objectAtIndex(0))) {
			CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, levelLayer));

		} else if (auto x = typeinfo_cast<LevelEditorLayer*>(CCScene::get()->getChildren()->objectAtIndex(0))) {
			geode::createQuickPopup(
				"HOLD ON!",            // title
				"You are about to be redirected out of the editor. Are you sure you want to save before exiting?",   // content
				"No", "Exit",      // buttons
				[levelLayer](auto, bool btn2) {
					if (btn2) {
						CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, levelLayer));
					}
				}
			);
		} else {
			CCDirector::sharedDirector()->pushScene(levelLayer);
		}
	});

	// Handles searching for players
	handleURI("user", [](std::string const& path) {
		GJSearchObject* x = GJSearchObject::create(SearchType::Users, path);
		auto levelLayer = LevelBrowserLayer::scene(x);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(.5f, levelLayer));
		// CCDirector::
		// auto scene = CCScene::get();
		// levelLayer->setTouchPriority(-500);
		// scene->addChild(levelLayer);
	});
};



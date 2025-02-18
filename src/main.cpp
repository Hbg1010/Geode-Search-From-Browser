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

$on_mod(Loaded) {
	handleURI("alert", [](std::string const& path) {
		FLAlertLayer::create("Custom Alert", path, "Ok")->show();
	});

	// Level searching: TODO figure out correct sorting with links
	handleURI("level", [](std::string const& path) {
		GJSearchObject* x = GJSearchObject::create(SearchType::Search, path);
		auto levelLayer = LevelBrowserLayer::create(x);
		auto scene = CCScene::get();
		levelLayer->setTouchPriority(-500);
		scene->addChild(levelLayer);
	});

	// Handles searching for players
	handleURI("user", [](std::string const& path) {
		GJSearchObject* x = GJSearchObject::create(SearchType::Users, path);
		auto levelLayer = LevelBrowserLayer::create(x);
		auto scene = CCScene::get();
		levelLayer->setTouchPriority(-500);
		scene->addChild(levelLayer);
	});
};



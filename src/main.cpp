#include <camila314.geode-uri/include/GeodeURI.hpp>

using namespace geode::prelude;

void addSearchLayer(SearchType type, const std::string& input) {
	if (GJBaseGameLayer::get()) return FLAlertLayer::create("Hey there!", fmt::format("Please exit the level before searching for the {} {}.", type == SearchType::Search ? "level" : "user", input), "OK")->show();
	auto search = GJSearchObject::create(type, input);
	auto levelLayer = LevelBrowserLayer::scene(search);
	CCDirector::sharedDirector()->pushScene(levelLayer);
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
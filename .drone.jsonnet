local image = "localhost:5000/jla/perm_group";
local boostArg(v) = "-DBOOST_ROOT=/opt/boost/%s" % v;

local CoverageStep(withCoverage, compiler, boost) = if !withCoverage then null else {
	name: "coverage",
	image: image,
	environment: {
		CTEST_OUTPUT_ON_FAILURE: 1,
		CXX: compiler,
		CXXFLAGS: "-Werror",
	},
	commands: [
		"mkdir covBuild",
		"cd covBuild",
		"cmake ../ -DBUILD_TESTING=on -DBUILD_COVERAGE=on %s" % [boostArg(boost)],
		"make",
		"make install",
		"make tests",
		"make coverage_collect",
		"make coverage_build",
		"/copyCoverage.sh",
	],
	volumes: [
		{
			name: "www",
			path: "/www",
		},
	],
};

local Volumes(withCoverage) = if !withCoverage then [] else [
	{
		name: "www",
		host: {
			path: "/www/results/perm_group",
		},
	},
];

local Pipeline(withCoverage, compiler, boost) = {
	kind: "pipeline",
	name: "%s, Boost %s" % [compiler, boost],
	steps: [
		{
			name: "bootstrap",
			image: image,
			commands: [
				"git fetch --tags",
				"git submodule update --init --recursive",
				"./bootstrap.sh",
			],
		},
		{
			name: "build",
			image: image,
			environment: {
				CXX: compiler,
				CXXFLAGS: "-Werror",
			},
			commands: [
				"mkdir build",
				"cd build",
				"cmake ../ -DBUILD_TESTING=on -DBUILD_TESTING_SANITIZERS=off %s" % [boostArg(boost)],
				"make",
			],
		},
		{
			name: "install",
			image: image,
			commands: [
				"cd build",
				"make install",
			],
		},
		{
			name: "build-test",
			image: image,
			commands: [
				"cd build",
				"make tests",
			],
		},
		{
			name: "test",
			image: image,
			commands: [
				"cd build",
				"make install",
				"ctest --output-on-failure",
			],
		},
		CoverageStep(withCoverage, compiler, boost),
	],
	volumes: Volumes(withCoverage),
};

[
	Pipeline(boost == "1_74_0" && compiler == "g++-9", compiler, boost)
	for compiler in [
		"g++-7", "g++-8", "g++-9", "g++-10",
		"clang++-6.0", "clang++-7", "clang++-8", "clang++-9", "clang++-10", "clang++-11"
	]
	for boost in [
		"1_67_0", "1_68_0", "1_69_0", "1_70_0", "1_71_0",
		"1_72_0", "1_73_0", "1_74_0", "1_75_0", "1_76_0",
	]
]

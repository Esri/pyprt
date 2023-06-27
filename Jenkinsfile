#!/usr/bin/env groovy

// This pipeline is designed to run on Esri-internal CI infrastructure.


// -- PIPELINE LIBRARIES

@Library('psl')
import com.esri.zrh.jenkins.PipelineSupportLibrary
import com.esri.zrh.jenkins.JenkinsTools
import com.esri.zrh.jenkins.ce.CityEnginePipelineLibrary
import com.esri.zrh.jenkins.ce.PrtAppPipelineLibrary as PAPL
import com.esri.zrh.jenkins.PslFactory
import com.esri.zrh.jenkins.psl.UploadTrackingPsl
import groovy.transform.Field

@Field def psl = PslFactory.create(this, UploadTrackingPsl.ID)
@Field def cepl = new CityEnginePipelineLibrary(this, psl)
@Field def papl = new PAPL(cepl)


// -- SETUP

properties([
	disableConcurrentBuilds()
])

psl.runsHere('production')
env.PIPELINE_ARCHIVING_ALLOWED = "true"


// -- GLOBAL DEFINITIONS

@Field final String REPO   = 'git@github.com:esri/pyprt.git'
@Field final String SOURCE = 'pyprt.git'
@Field final String CREDS = 'jenkins-devtopia-pyprt-deployer-key'
@Field final String SOURCE_STASH = 'pyprt-sources'
@Field String pkgVer = "0.0.0"

@Field final String DOCKER_IMAGE_REV = "v7"

@Field final String DOCKER_AGENT_LINUX = 'centos7-64-d'
@Field final String DOCKER_WS_LINUX = "/tmp/pyprt/ws"

@Field final String DOCKER_AGENT_WINDOWS = 'win19-64-d'
@Field final String DOCKER_WS_WINDOWS = "c:/temp/pyprt/ws"

@Field final Map PY37                  = [ py: '3.7' ]
@Field final Map PY38                  = [ py: '3.8' ]
@Field final Map PY39                  = [ py: '3.9' ]
@Field final Map PY310                 = [ py: '3.10' ]
@Field final Map KIND_WHEEL            = [ kind: 'wheel' ]
@Field final Map KIND_CONDA            = [ kind: 'conda' ]
@Field final Map LINUX_NATIVE_CONFIG   = [ os: cepl.CFG_OS_RHEL7, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_GCC93, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64 ]
@Field final Map WINDOWS_NATIVE_CONFIG = [ os: cepl.CFG_OS_WIN10, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_VC1427, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64 ]
@Field final Map LINUX_DOCKER_CONFIG   = [ ba: DOCKER_AGENT_LINUX, ws: DOCKER_WS_LINUX ]
@Field final Map WINDOWS_DOCKER_CONFIG = [ ba: DOCKER_AGENT_WINDOWS, ws: DOCKER_WS_WINDOWS ]

@Field final Map PRT_DEFAULT           = [ prt: 'Default' ] // as defined in the build system
@Field final Map PRT_LATEST            = [ prt: 'Latest' ] // latest internal PRT build

@Field final List CONFIGS_PREPARE = [
	composeConfig(PY38, KIND_WHEEL, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
]

@Field final List CONFIGS_TEST = [
	composeConfig(PY37, KIND_WHEEL, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
	composeConfig(PY38, KIND_WHEEL, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
	composeConfig(PY38, KIND_WHEEL, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
	composeConfig(PY39, KIND_WHEEL, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
	composeConfig(PY39, KIND_WHEEL, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
	composeConfig(PY310, KIND_WHEEL, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
	composeConfig(PY310, KIND_WHEEL, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
	composeConfig(PY310, KIND_WHEEL, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG, PRT_LATEST),
	composeConfig(PY310, KIND_WHEEL, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG, PRT_LATEST),
]

@Field final List CONFIGS_BUILD_WHEEL = [
	composeConfig(PY37, KIND_WHEEL, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
	composeConfig(PY38, KIND_WHEEL, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
	composeConfig(PY38, KIND_WHEEL, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
	composeConfig(PY39, KIND_WHEEL, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
	composeConfig(PY39, KIND_WHEEL, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
	composeConfig(PY310, KIND_WHEEL, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
	composeConfig(PY310, KIND_WHEEL, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
	composeConfig(PY310, KIND_WHEEL, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG, PRT_LATEST),
	composeConfig(PY310, KIND_WHEEL, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG, PRT_LATEST),
]

@Field final List CONFIGS_BUILD_CONDA = [
	composeConfig(PY37, KIND_CONDA, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
	composeConfig(PY37, KIND_CONDA, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
	composeConfig(PY38, KIND_CONDA, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
	composeConfig(PY38, KIND_CONDA, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
	composeConfig(PY39, KIND_CONDA, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
	composeConfig(PY39, KIND_CONDA, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
	composeConfig(PY310, KIND_CONDA, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
	composeConfig(PY310, KIND_CONDA, WINDOWS_NATIVE_CONFIG, WINDOWS_DOCKER_CONFIG),
]

@Field final List CONFIGS_DOC = [
	composeConfig(PY38, KIND_WHEEL, LINUX_NATIVE_CONFIG, LINUX_DOCKER_CONFIG),
]


// -- PIPELINE

stage('prepare') {
	cepl.runParallel(taskGenPrepare())
}

stage('test') {
	cepl.runParallel(taskGenTests())
}

stage('build') {
	cepl.runParallel(taskGenPyPRT())
}

papl.finalizeRun('pyprt', env.BRANCH_NAME)


// -- TASK GENERATORS

Map taskGenPrepare() {
	Map tasks = [:]
	tasks << cepl.generateTasks('prepare', this.&taskPrepare, CONFIGS_PREPARE)
	return tasks
}

Map taskGenTests() {
	Map tasks = [:]
	tasks << cepl.generateTasks('test', this.&taskRunTests, CONFIGS_TEST)
	return tasks
}

Map taskGenPyPRT() {
	Map tasks = [:]
	tasks << cepl.generateTasks('build', this.&taskBuildWheel, CONFIGS_BUILD_WHEEL)
	tasks << cepl.generateTasks('build', this.&taskBuildConda, CONFIGS_BUILD_CONDA)
	tasks << cepl.generateTasks('doc', this.&taskBuildDoc, CONFIGS_DOC)
	return tasks;
}


// -- TASK BUILDERS

def taskPrepare(cfg) {
	cepl.cleanCurrentDir()
	papl.checkout(REPO, env.BRANCH_NAME, CREDS)

	def deps = readProperties(file: "${SOURCE}/src/dependencies.properties")

	dir(path: "${SOURCE}/src") {
		String tmpPath = pwd(tmp: true)
		Map PYBIND11_CONFIG = [ r: 'thirdparty', g: 'com.github', a: 'pybind11', v: deps.PYBIND11_VERSION, e: 'zip', f: "${tmpPath}/pybind11.zip", extract: true ]
		psl.fetchFromNexus2(PYBIND11_CONFIG)
		dir(path: tmpPath) {
			deleteDir() // remove archive zip
		}
	}

	dir(path: 'cesdk_latest') {
		dir(path: 'windows') {
			def myCfg = cfg + WINDOWS_NATIVE_CONFIG
			papl.fetchDependency(PAPL.Dependencies.CESDK_LATEST, myCfg)
		}
		dir(path: 'linux') {
			def myCfg = cfg + LINUX_NATIVE_CONFIG
			papl.fetchDependency(PAPL.Dependencies.CESDK_LATEST, myCfg)
		}
	}

	dir(path: 'cesdk_default') {
		String CESDK_BASE_URL = 'https://github.com/Esri/cityengine-sdk/releases/download'
		dir(path: 'windows') {
			String CESDK_URL_WINDOWS = "${CESDK_BASE_URL}/${deps.PRT_VERSION}/esri_ce_sdk-${deps.PRT_VERSION}-${deps.PRT_CLS_WINDOWS}.zip"
			downloadAndExtract(CESDK_URL_WINDOWS)
		}
		dir(path: 'linux') {
			String CESDK_URL_LINUX = "${CESDK_BASE_URL}/${deps.PRT_VERSION}/esri_ce_sdk-${deps.PRT_VERSION}-${deps.PRT_CLS_LINUX}.zip"
			downloadAndExtract(CESDK_URL_LINUX)
		}
	}

	stash(name: SOURCE_STASH)

	String buildCmd = "python setup.py build_py && python get_pkg_version.py > ${cfg.ws}/current_version.txt"
	String workDir = "${cfg.ws}/${SOURCE}"
	Map dirMap = [ (env.WORKSPACE) : cfg.ws ]
	runDockerCmd(cfg, dirMap, workDir, buildCmd)

	String rawVer = readFile(file: "current_version.txt")
	pkgVer = "${rawVer.trim()}-${env.BUILD_NUMBER}"
	echo("Detected PyPRT version: ${pkgVer}")
}

def taskBuildWheel(cfg) {
	cepl.cleanCurrentDir()
	unstash(name: SOURCE_STASH)

	String buildCmd = "python setup.py bdist_wheel --dist-dir=${cfg.ws}/build --build-number=${env.BUILD_NUMBER}"
	if (isUnix())
		buildCmd += ' -p manylinux2014_x86_64' // see https://github.com/pypa/manylinux

	String workDir = "${cfg.ws}/${SOURCE}"
	Map dirMap = [ (env.WORKSPACE) : cfg.ws ]
	runDockerCmd(cfg, dirMap, workDir, updateBuildEnv(cfg, workDir, buildCmd))

	def classifierExtractor = { p ->
		def clsRegEx = (p =~ /[^-]*-[^-]*-[0-9]*-([^-]*-[^-]*-[^-]*)\.whl/)
		String cls = clsRegEx[0][1]
		if (cfg.prt == PRT_LATEST.prt)
			cls += '-prtLatest'
		return cls
	}
	papl.publish('pyprt', env.BRANCH_NAME, "PyPRT-*.whl", { return pkgVer }, cfg, classifierExtractor)
}

def taskBuildConda(cfg) {
	cepl.cleanCurrentDir()
	unstash(name: SOURCE_STASH)

	String buildCmd = "python setup.py bdist_conda --buildnum=${env.BUILD_NUMBER}"
	if (isUnix()) {
		String condaEnv = '/tmp/pyprt/pyprt-conda-env'
		String outDir = "${cfg.ws}/build/"
		buildCmd += " && mkdir ${outDir} && cp -r ${condaEnv}/conda-bld/linux-64/pyprt*.tar.bz2 ${outDir}"
	}
	else {
		String condaEnv = 'C:\\temp\\conda\\envs\\pyprt'
		String outDir = "${cfg.ws.replace('/','\\')}\\build\\"
		buildCmd += " && mkdir ${outDir} && copy ${condaEnv}\\conda-bld\\win-64\\pyprt-*.tar.bz2 ${outDir}"
	}

	String workDir = "${cfg.ws}/${SOURCE}"
	Map dirMap = [ (env.WORKSPACE) : cfg.ws ]
	runDockerCmd(cfg, dirMap, workDir, updateBuildEnv(cfg, workDir, buildCmd))

	def classifierExtractor = { p ->
		def cls = (p =~ /.*-(py[0-9]+)_[0-9]+\.tar\.bz2/)
		return "${cls[0][1]}-${cfg.os}-${cfg.arch}"
	}
	papl.publish('pyprt', env.BRANCH_NAME, "pyprt-*.tar.bz2", { return pkgVer }, cfg, classifierExtractor)
}

def taskBuildDoc(cfg) {
	cepl.cleanCurrentDir()
	unstash(name: SOURCE_STASH)

	final String sphinxOutput = "${env.WORKSPACE}/build"

	String buildLib = "${cfg.ws}/tmp_build"
	String buildResult = "${cfg.ws}/build"
	String buildCmd = "python setup.py build --build-lib=${buildLib} && PYPRT_PACKAGE_LOCATION=${buildLib} python setup.py build_doc --build-dir=${buildResult}"
	String workDir = "${cfg.ws}/${SOURCE}"
	Map dirMap = [ (env.WORKSPACE) : cfg.ws ]
	runDockerCmd(cfg, dirMap, workDir, buildCmd)

	dir(path: sphinxOutput) {
		zip(zipFile: "pyprt-doc.zip", dir: "html")
	}

	papl.publish('pyprt', env.BRANCH_NAME, "pyprt-doc.zip", { return pkgVer }, cfg, { return "doc" })
}

def taskRunTests(cfg) {
	cepl.cleanCurrentDir()
	unstash(name: SOURCE_STASH)

	String buildCmd = "python setup.py install && python tests/run_tests.py --xml_output_directory ${cfg.ws}"
	String workDir = "${cfg.ws}/${SOURCE}"
	Map dirMap = [ (env.WORKSPACE) : cfg.ws ]
	runDockerCmd(cfg, dirMap, workDir, updateBuildEnv(cfg, workDir, buildCmd))

	junit(testResults: 'TEST-*.xml')
}


// -- HELPERS

String updateBuildEnv(Map cfg, String workDir, String buildCmd) {
	def deps = readProperties(file: "${SOURCE}/src/dependencies.properties")

	String pybind11Env = "PYBIND11_DIR=${workDir}/src/pybind11-${deps.PYBIND11_VERSION}"

	String os = isUnix() ? 'linux' : 'windows'
	String cesdkDir = (cfg.prt == PRT_LATEST.prt) ? "cesdk_latest/${os}/ce_sdk" : "cesdk_default/${os}"
	String cesdkEnv = "PRT_DIR=${cfg.ws}/${cesdkDir}/cmake"

	String envCmd = isUnix() ? "export ${pybind11Env} ${cesdkEnv} " : "set ${pybind11Env}&& set ${cesdkEnv}"

	return "${envCmd}&& ${buildCmd}"
}

@NonCPS
Map composeConfig(py, kind, tc, dc, prt = PRT_DEFAULT) {
	String label = "py${py['py']}-${kind['kind']}"
	if (prt != PRT_DEFAULT)
		label += "-prt${prt['prt']}"
	return py + kind + tc + dc + prt + [ grp: label ]
}

String getDockerImage(Map cfg) {
	String image = 'zrh-dreg-sp-1.esri.com/pyprt/pyprt'

	String tag = "jnk-${DOCKER_IMAGE_REV}-"
	tag += (cfg.os == cepl.CFG_OS_WIN10) ? 'windows' : (cfg.os == cepl.CFG_OS_RHEL7) ? 'centos7' : error(cfg.os)
	tag += "-py${cfg.py}-${cfg.kind}-${cfg.tc}"

	return "${image}:${tag}"
}

def runDockerCmd(Map cfg, Map dirMap, String workDir, String cmd) {
	String dirMapStrArgs = ""
	dirMap.each { k,v -> dirMapStrArgs += " -v \"${k}:${v}\"" }

	String runArgs = '--pull always --rm'
	runArgs += " --name pyprt"
	runArgs += dirMapStrArgs
	runArgs += " -w ${workDir}"
	runArgs += " ${getDockerImage(cfg)}"
	runArgs += isUnix() ? " bash -c '${cmd}'" : " cmd /c \"${cmd}\""

	psl.runCmd("docker run ${runArgs}")
}

def downloadFile(String url, String dest, int numRetries = 5, int retryMaxTimeSecs = 180) {
	def curlCmd = "curl --location --remote-header-name --retry ${numRetries} --retry-max-time ${retryMaxTimeSecs} -o ${dest} ${url}"

	try {
		psl.runCmd(curlCmd)
	} catch(e) {
		String reason =
			e.message && e.message.endsWith('exit code 22') ? "Requested document could not be downloaded within ${retryMaxTimeSecs} secs." : e.message
		error("Failed to download ${url}: ${reason}")
	}

	if (!fileExists(dest)) {
		error("Failed to download: ${url}")
	}
}

def downloadAndExtract(String url) {
	String tmpPath = pwd(tmp: true)
	String archive = "${tmpPath}/archive.zip"
	downloadFile(url, archive)
	unzip(zipFile: archive)
	dir(path: tmpPath) {
		deleteDir() // remove archive zip
	}
}

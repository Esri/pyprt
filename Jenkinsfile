#!/usr/bin/env groovy

// This pipeline is designed to run on Esri-internal CI infrastructure.


// -- PIPELINE LIBRARIES

@Library('psl')
import com.esri.zrh.jenkins.PipelineSupportLibrary
import com.esri.zrh.jenkins.JenkinsTools
import com.esri.zrh.jenkins.ce.CityEnginePipelineLibrary
import com.esri.zrh.jenkins.ce.PrtAppPipelineLibrary
import com.esri.zrh.jenkins.PslFactory
import com.esri.zrh.jenkins.psl.UploadTrackingPsl
import groovy.transform.Field

@Field def psl = PslFactory.create(this, UploadTrackingPsl.ID)
@Field def cepl = new CityEnginePipelineLibrary(this, psl)
@Field def papl = new PrtAppPipelineLibrary(cepl)


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

@Field final String DOCKER_IMAGE_REV = "v4"

@Field final String DOCKER_AGENT_LINUX = 'centos7-64-d'
@Field final String DOCKER_WS_LINUX = "/tmp/pyprt/ws"

@Field final String DOCKER_AGENT_WINDOWS = 'win19-64-d'
@Field final String DOCKER_WS_WINDOWS = "c:/temp/pyprt/ws"

@Field final Map PY36_CONFIG           = [ py: '3.6-wheel' ]
@Field final Map PY37_CONFIG           = [ py: '3.7-wheel' ]
@Field final Map PY38_CONFIG           = [ py: '3.8-wheel' ]
@Field final Map PY39_CONFIG           = [ py: '3.9-wheel' ]
@Field final Map PY36_CONDA_CONFIG     = [ py: '3.6-conda' ]
@Field final Map PY37_CONDA_CONFIG     = [ py: '3.7-conda' ]
@Field final Map PY38_CONDA_CONFIG     = [ py: '3.8-conda' ]
@Field final Map PY39_CONDA_CONFIG     = [ py: '3.9-conda' ]
@Field final Map LINUX_NATIVE_CONFIG   = [ os: cepl.CFG_OS_RHEL7, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_GCC93, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64 ]
@Field final Map WINDOWS_NATIVE_CONFIG = [ os: cepl.CFG_OS_WIN10, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_VC1427, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64 ]
@Field final Map LINUX_DOCKER_CONFIG   = [ ba: DOCKER_AGENT_LINUX, ws: DOCKER_WS_LINUX ]
@Field final Map WINDOWS_DOCKER_CONFIG = [ ba: DOCKER_AGENT_WINDOWS, ws: DOCKER_WS_WINDOWS ]

@Field final List CONFIGS_PREPARE = [
	PY36_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
]

@Field final List CONFIGS_TESTS_PY36 = [
	PY36_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
 	PY36_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_TESTS_PY37 = [
 	PY37_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_TESTS_PY38 = [
	PY38_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
	PY38_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_TESTS_PY39 = [
	PY39_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
	PY39_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_BUILD_WHEELS_PY36 = [
	PY36_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
	PY36_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_BUILD_WHEELS_PY37 = [
	PY37_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_BUILD_WHEELS_PY38 = [
	PY38_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
	PY38_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_BUILD_WHEELS_PY39 = [
	PY39_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
	PY39_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_BUILD_CONDA_PY36 = [
	PY36_CONDA_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
	PY36_CONDA_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_BUILD_CONDA_PY37 = [
	PY37_CONDA_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
	PY37_CONDA_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_BUILD_CONDA_PY38 = [
	PY38_CONDA_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
	PY38_CONDA_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_BUILD_CONDA_PY39 = [
	PY39_CONDA_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
	PY39_CONDA_CONFIG + WINDOWS_DOCKER_CONFIG + WINDOWS_NATIVE_CONFIG,
]

@Field final List CONFIGS_DOC = [
	PY36_CONFIG + LINUX_DOCKER_CONFIG + LINUX_NATIVE_CONFIG,
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
	tasks << cepl.generateTasks('pyprt-prepare', this.&taskPrepare, CONFIGS_PREPARE)
	return tasks
}

Map taskGenTests() {
	Map tasks = [:]
 	tasks << cepl.generateTasks('pyprt-tests-py36', this.&taskRunTests, CONFIGS_TESTS_PY36)
 	tasks << cepl.generateTasks('pyprt-tests-py37', this.&taskRunTests, CONFIGS_TESTS_PY37)
 	tasks << cepl.generateTasks('pyprt-tests-py38', this.&taskRunTests, CONFIGS_TESTS_PY38)
 	tasks << cepl.generateTasks('pyprt-tests-py39', this.&taskRunTests, CONFIGS_TESTS_PY39)
	return tasks
}

Map taskGenPyPRT() {
	Map tasks = [:]
  	tasks << cepl.generateTasks('pyprt-wheel-py36', this.&taskBuildWheel, CONFIGS_BUILD_WHEELS_PY36)
  	tasks << cepl.generateTasks('pyprt-wheel-py37', this.&taskBuildWheel, CONFIGS_BUILD_WHEELS_PY37)
  	tasks << cepl.generateTasks('pyprt-wheel-py38', this.&taskBuildWheel, CONFIGS_BUILD_WHEELS_PY38)
  	tasks << cepl.generateTasks('pyprt-wheel-py39', this.&taskBuildWheel, CONFIGS_BUILD_WHEELS_PY39)
	tasks << cepl.generateTasks('pyprt-conda-py36', this.&taskBuildConda, CONFIGS_BUILD_CONDA_PY36)
 	tasks << cepl.generateTasks('pyprt-conda-py37', this.&taskBuildConda, CONFIGS_BUILD_CONDA_PY37)
 	tasks << cepl.generateTasks('pyprt-conda-py38', this.&taskBuildConda, CONFIGS_BUILD_CONDA_PY38)
 	tasks << cepl.generateTasks('pyprt-conda-py39', this.&taskBuildConda, CONFIGS_BUILD_CONDA_PY39)
  	tasks << cepl.generateTasks('pyprt-doc', this.&taskBuildDoc, CONFIGS_DOC)
	return tasks;
}


// -- TASK BUILDERS

def taskPrepare(cfg) {
 	cepl.cleanCurrentDir()
	papl.checkout(REPO, env.BRANCH_NAME, CREDS)
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
	runDockerCmd(cfg, dirMap, workDir, buildCmd)

	def classifierExtractor = { p ->
		def cls = (p =~ /[^-]*-[^-]*-[0-9]*-([^-]*-[^-]*-[^-]*)\.whl/)
		return cls[0][1]
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
	runDockerCmd(cfg, dirMap, workDir, buildCmd)

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
	runDockerCmd(cfg, dirMap, workDir, buildCmd)

	junit(testResults: 'TEST-*.xml')
}


// -- HELPERS

String getPySuf(cfg) {
	return cfg.py.replace(".", "")
}

String getDockerEnvDir(Map cfg) {
	String envDir = "envs/"
	switch (cfg.os) {
		case cepl.CFG_OS_WIN10: envDir += 'windows'; break;
		case cepl.CFG_OS_RHEL7: envDir += 'centos7'; break;
		default: error("No docker env available for ${cfg.os}")
	}
	return "${envDir}/py${getPySuf(cfg)}"
}

String getDockerImage(Map cfg) {
	String image = 'zrh-dreg-sp-1.esri.com/pyprt/pyprt'

	String tag = "jnk-${DOCKER_IMAGE_REV}-"
	tag += (cfg.os == cepl.CFG_OS_WIN10) ? 'windows' : (cfg.os == cepl.CFG_OS_RHEL7) ? 'centos7' : error(cfg.os)
	tag += "-py${cfg.py}-${cfg.tc}"

	return "${image}:${tag}"
}

def runDockerCmd(Map cfg, Map dirMap, String workDir, String cmd) {
	String dirMapStrArgs = ""
	dirMap.each { k,v -> dirMapStrArgs += " -v \"${k}:${v}\"" }

	String runArgs = '--pull always --rm'
	runArgs += dirMapStrArgs
	runArgs += " -w ${workDir}"
	runArgs += " ${getDockerImage(cfg)}"
	runArgs += isUnix() ? " bash -c '${cmd}'" : " cmd /c \"${cmd}\""

	psl.runCmd("docker run ${runArgs}")
}

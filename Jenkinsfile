#!/usr/bin/env groovy

// This pipeline is designed to run on Esri-internal CI infrastructure.


// -- PIPELINE LIBRARIES

@Library('psl')
import com.esri.zrh.jenkins.PipelineSupportLibrary
import com.esri.zrh.jenkins.JenkinsTools
import com.esri.zrh.jenkins.ce.CityEnginePipelineLibrary
import com.esri.zrh.jenkins.ce.PrtAppPipelineLibrary
import groovy.transform.Field

@Field def psl = new PipelineSupportLibrary(this)
@Field def cepl = new CityEnginePipelineLibrary(this, psl)
@Field def papl = new PrtAppPipelineLibrary(cepl)


// -- SETUP

properties([
	disableConcurrentBuilds()
])

psl.runsHere('production')
env.PIPELINE_ARCHIVING_ALLOWED = "true"


// -- GLOBAL DEFINITIONS

@Field final String REPO   = 'git@devtopia.esri.com:Zurich-R-D-Center/pyprt.git'
@Field final String SOURCE = 'pyprt.git'

@Field final List CONFIGS_PY36 = [
	[ os: cepl.CFG_OS_RHEL7, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_GCC83, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64, python: '3.6' ],
	[ os: cepl.CFG_OS_WIN10, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_VC142, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64, python: '3.6' ],
]


// -- PIPELINE

stage('PyPRT') {
	cepl.runParallel(getTasks())
}

Map getTasks() {
	Map tasks = [:]
	tasks << taskGenPyPRT()
	return tasks
}


// -- TASK GENERATORS

Map taskGenPyPRT() {
	Map tasks = [:]
	tasks << cepl.generateTasks('PyPRT-py36', this.&taskBuildPyPRT, CONFIGS_PY36)
	return tasks;
}


// -- TASK BUILDERS

def taskBuildPyPRT(cfg) {
	List deps = []
	List defs = []

	cepl.cleanCurrentDir()
	papl.checkout(REPO, env.BRANCH_NAME)

	final JenkinsTools toolchain = cepl.getToolchainTool(cfg)
	final List envTools = [JenkinsTools.CMAKE313, JenkinsTools.NINJA, toolchain]
	List buildEnvs = JenkinsTools.generateToolEnv(this, envTools)
	dir(path: SOURCE) {
		withEnv(buildEnvs) {
			String cmd = toolchain.getSetupCmd(this, cfg)
			cmd += "\npipenv run python setup.py bdist_wheel --dist-dir=${env.WORKSPACE}/build --build-number=${env.BUILD_NUMBER}"
			psl.runCmd(cmd)
		}
	}

	final String pkgInfo = readFile(file: "${SOURCE}/PyPRT.egg-info/PKG-INFO")
	def versionMatcher = (pkgInfo =~ /(?m)^Version: (.*)$/)
	final String pkgVersion = "${versionMatcher[0][1]}-${env.BUILD_NUMBER}"
	final String classifier = "py${cfg.python}-${cepl.getArchiveClassifier(cfg)}"

	def versionExtractor = { return pkgVersion }
	def classifierExtractor = { return classifier }
	papl.publish('pyprt', env.BRANCH_NAME, "PyPRT-*.whl", versionExtractor, cfg, classifierExtractor)
}

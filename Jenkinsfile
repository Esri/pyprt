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

@Field final List CONFIGS_PY36 = [
	[ os: cepl.CFG_OS_RHEL7, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_GCC83, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64, python: '3.6' ],
	[ os: cepl.CFG_OS_WIN10, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_VC142, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64, python: '3.6' ],
]

@Field final List CONFIGS_DOC = [
	[ os: cepl.CFG_OS_RHEL7, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_GCC83, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64, python: '3.6' ],
]


// -- PIPELINE

stage('pyprt') {
	cepl.runParallel(getTasks())
	papl.finalizeRun('pyprt', env.BRANCH_NAME)
}

Map getTasks() {
	Map tasks = [:]
	tasks << taskGenPyPRT()
	return tasks
}


// -- TASK GENERATORS

Map taskGenPyPRT() {
	Map tasks = [:]
	tasks << cepl.generateTasks('pyprt-py36', this.&taskBuildPyPRT, CONFIGS_PY36)
	tasks << cepl.generateTasks('pyprt-py36-conda', this.&taskCondaBuildPyPRT, CONFIGS_PY36)
	tasks << cepl.generateTasks('pyprt-doc', this.&taskBuildDoc, CONFIGS_DOC)
	return tasks;
}


// -- TASK BUILDERS

def taskBuildPyPRT(cfg) {
	cepl.cleanCurrentDir()
	papl.checkout(REPO, env.BRANCH_NAME)

	final JenkinsTools toolchain = cepl.getToolchainTool(cfg)
	final List envTools = [JenkinsTools.CMAKE313, JenkinsTools.NINJA, toolchain]
	List buildEnvs = JenkinsTools.generateToolEnv(this, envTools)
	dir(path: SOURCE) {
		withEnv(buildEnvs + ["PIPENV_DEFAULT_PYTHON_VERSION=${cfg.python}"]) {
			psl.runCmd("pipenv install")

			String cmd = toolchain.getSetupCmd(this, cfg)
			cmd += "\npipenv run pip list"
			cmd += "\npipenv run python setup.py bdist_wheel --dist-dir=${env.WORKSPACE}/build --build-number=${env.BUILD_NUMBER}"
			psl.runCmd(cmd)
		}
	}

	def versionExtractor = { p ->
		def vers = (p =~ /.*PyPRT-([0-9]+\.[0-9]+\.[0-9abpr]+-[0-9]+)-cp.*/)
		return vers[0][1]
	}
	def classifierExtractor = { p ->
		def cls = (p =~ /.*PyPRT-[0-9]+\.[0-9]+\.[0-9abpr]+-[0-9]+-(.*)\.whl/)
		return cls[0][1]
	}
	papl.publish('pyprt', env.BRANCH_NAME, "PyPRT-*.whl", versionExtractor, cfg, classifierExtractor)
}

def taskCondaBuildPyPRT(cfg) {
	cepl.cleanCurrentDir()
	papl.checkout(REPO, env.BRANCH_NAME)

	final JenkinsTools toolchain = cepl.getToolchainTool(cfg)
	final JenkinsTools CONDA = JenkinsTools.CONDA
	final String condaToolEnvVar = CONDA.getEvalEnv(this)

	final String condaEnvName = 'pyprt-conda-env'
	final String condaEnvPath = "${env.WORKSPACE}/${condaEnvName}"
	final String condaNativeEnvPath = isUnix() ? condaEnvPath : condaEnvPath.replaceAll('/', '\\\\')
	final String condaNativeEnvCondaCmd = isUnix() ? "${condaEnvPath}/bin/conda" : "${condaEnvPath}\\condabin\\conda.bat".replaceAll('/', '\\\\')

	final List envTools = [JenkinsTools.CMAKE313, JenkinsTools.NINJA, toolchain, CONDA]
	final List buildEnvs = JenkinsTools.generateToolEnv(this, envTools)
	dir(path: SOURCE) {
		withEnv(buildEnvs) {
			final String condaCmd = CONDA.getCmd(this)

			psl.runCmd("${condaCmd} env create -f environment.yml -p ${condaNativeEnvPath}")
			psl.runCmd("${condaNativeEnvCondaCmd} list")

			final String cmd = toolchain.getSetupCmd(this, cfg)
			cmd += "\n${condaNativeEnvCondaCmd} run python setup.py bdist_conda --buildnum=${env.BUILD_NUMBER}"
			psl.runCmd(cmd)
		}
	}

	// cannot control where conda outputs the package, need to adapt to expectations of publish
	fileOperations([
	    folderCreateOperation('build'),
	    fileCopyOperation(
			includes: "${condaEnvName}/conda-bld/*/pyprt-*.tar.bz2",
			excludes: '',
			targetLocation: 'build',
			flattenFiles: true
		)
	])

 	def versionExtractor = { p ->
 		def vers = (p =~ /.*pyprt-([0-9]+\.[0-9]+\.[0-9abpr]+)-py[0-9]+_([0-9]+)\.tar\.bz2/)
 		return "${vers[0][1]}-${vers[0][2]}"
 	}
	def classifierExtractor = { p ->
 		def cls = (p =~ /.*pyprt-[0-9]+\.[0-9]+\.[0-9abpr]+-(py[0-9]+)_[0-9]+\.tar\.bz2/)
		return "${cls[0][1]}-${cfg.os}-${cfg.arch}"
	}
 	papl.publish('pyprt', env.BRANCH_NAME, "pyprt-*.tar.bz2", versionExtractor, cfg, classifierExtractor)
}

def taskBuildDoc(cfg) {
	cepl.cleanCurrentDir()
	papl.checkout(REPO, env.BRANCH_NAME)

	final String sphinxOutput = "${env.WORKSPACE}/build"
	String pkgVer

	final JenkinsTools toolchain = cepl.getToolchainTool(cfg)
	final List envTools = [JenkinsTools.CMAKE313, JenkinsTools.NINJA, toolchain]
	List buildEnvs = JenkinsTools.generateToolEnv(this, envTools)
	dir(path: SOURCE) {
		withEnv(buildEnvs + ["PIPENV_DEFAULT_PYTHON_VERSION=${cfg.python}"]) {
			psl.runCmd("pipenv install")

			final String buildLib = pwd(tmp: true)

			String cmd = toolchain.getSetupCmd(this, cfg)
			cmd += "\npipenv run python setup.py build --build-lib=${buildLib}"
			cmd += "\nPYPRT_PACKAGE_LOCATION=${buildLib} pipenv run python setup.py build_doc --build-dir=${sphinxOutput}"
			psl.runCmd(cmd)

			pkgVer = psl.runCmd('pipenv run python get_pkg_version.py', true)
		}
	}

	dir(path: sphinxOutput) {
		zip(zipFile: "pyprt-doc.zip", dir: "html")
	}

	assert pkgVer != null
	papl.publish('pyprt-doc', env.BRANCH_NAME, "pyprt-doc.zip", { return "${pkgVer}-${env.BUILD_NUMBER}" }, cfg)
}

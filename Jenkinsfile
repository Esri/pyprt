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

@Field final List CONFIGS_PREPARE = [
	[ os: cepl.CFG_OS_RHEL7, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_GCC83, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64, python: '3.6' ],
]

@Field final List CONFIGS_PY36 = [
	[ os: cepl.CFG_OS_RHEL7, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_GCC83, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64, python: '3.6' ],
	[ os: cepl.CFG_OS_WIN10, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_VC142, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64, python: '3.6' ],
]

@Field final List CONFIGS_DOC = [
	[ os: cepl.CFG_OS_RHEL7, bc: cepl.CFG_BC_REL, tc: cepl.CFG_TC_GCC83, cc: cepl.CFG_CC_OPT, arch: cepl.CFG_ARCH_X86_64, python: '3.6' ],
]


// -- PIPELINE

stage('prepare') {
	cepl.runParallel(taskGenPrepare())
}

stage('pyprt') {
	cepl.runParallel(taskGenPyPRT())
}

papl.finalizeRun('pyprt', env.BRANCH_NAME)


// -- TASK GENERATORS

Map taskGenPrepare() {
	Map tasks = [:]
	tasks << cepl.generateTasks('pyprt-prepare', this.&taskPrepare, CONFIGS_PREPARE)
	return tasks
}

Map taskGenPyPRT() {
	Map tasks = [:]
	tasks << cepl.generateTasks('pyprt-py36', this.&taskBuildPyPRT, CONFIGS_PY36)
	tasks << cepl.generateTasks('pyprt-py36-conda', this.&taskCondaBuildPyPRT, CONFIGS_PY36)
	tasks << cepl.generateTasks('pyprt-doc', this.&taskBuildDoc, CONFIGS_DOC)
	return tasks;
}


// -- TASK BUILDERS

@Field String pkgVer = "0.0.0"

def taskPrepare(cfg) {
 	cepl.cleanCurrentDir()
	papl.checkout(REPO, env.BRANCH_NAME, CREDS)

 	dir(path: SOURCE) {
		final String pyCmd = setupPythonEnv(cfg)
		psl.runCmd("${pyCmd} setup.py build_py")
		pkgVer = psl.runCmd("${pyCmd} get_pkg_version.py", true)
        pkgVer += "-${env.BUILD_NUMBER}"
 	}

 	echo("Detected PyPRT version: ${pkgVer}")
}

def taskBuildPyPRT(cfg) {
	cepl.cleanCurrentDir()
	papl.checkout(REPO, env.BRANCH_NAME, CREDS)

	final JenkinsTools toolchain = cepl.getToolchainTool(cfg)
	final List envTools = [JenkinsTools.CMAKE313, JenkinsTools.NINJA, toolchain]
	List buildEnvs = JenkinsTools.generateToolEnv(this, envTools)
	dir(path: SOURCE) {
		withEnv(buildEnvs) {
			final String pyCmd = setupPythonEnv(cfg)
			String cmd = toolchain.getSetupCmd(this, cfg)
			cmd += "\n${pyCmd} setup.py bdist_wheel --dist-dir=${env.WORKSPACE}/build --build-number=${env.BUILD_NUMBER}"
			psl.runCmd(cmd)
		}
	}

	def classifierExtractor = { p ->
		def cls = (p =~ /[^-]*-[^-]*-[0-9]*-([^-]*-[^-]*-[^-]*)\.whl/)
		return cls[0][1]
	}
	papl.publish('pyprt', env.BRANCH_NAME, "PyPRT-*.whl", { return pkgVer }, cfg, classifierExtractor)
}

def taskCondaBuildPyPRT(cfg) {
	cepl.cleanCurrentDir()
	papl.checkout(REPO, env.BRANCH_NAME, CREDS)

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

	def classifierExtractor = { p ->
 		def cls = (p =~ /.*-(py[0-9]+)_[0-9]+\.tar\.bz2/)
		return "${cls[0][1]}-${cfg.os}-${cfg.arch}"
	}
 	papl.publish('pyprt', env.BRANCH_NAME, "pyprt-*.tar.bz2", { return pkgVer }, cfg, classifierExtractor)
}

def taskBuildDoc(cfg) {
	cepl.cleanCurrentDir()
	papl.checkout(REPO, env.BRANCH_NAME, CREDS)

	final String sphinxOutput = "${env.WORKSPACE}/build"

	final JenkinsTools toolchain = cepl.getToolchainTool(cfg)
	final List envTools = [JenkinsTools.CMAKE313, JenkinsTools.NINJA, toolchain]
	List buildEnvs = JenkinsTools.generateToolEnv(this, envTools)
	dir(path: SOURCE) {
		withEnv(buildEnvs) {
			final String pyCmd = setupPythonEnv(cfg)
			final String buildLib = pwd(tmp: true)

			String cmd = toolchain.getSetupCmd(this, cfg)
			cmd += "\n${pyCmd} setup.py build --build-lib=${buildLib}"
			cmd += "\nPYPRT_PACKAGE_LOCATION=${buildLib} ${pyCmd} setup.py build_doc --build-dir=${sphinxOutput}"
			psl.runCmd(cmd)
		}
	}

	dir(path: sphinxOutput) {
		zip(zipFile: "pyprt-doc.zip", dir: "html")
	}

	papl.publish('pyprt', env.BRANCH_NAME, "pyprt-doc.zip", { return pkgVer }, cfg, { return "doc" })
}


// -- HELPERS

String setupPythonEnv(cfg) {
	final String envName = 'pyprt-venv'
	final String envPath = "${env.WORKSPACE}/${envName}"

	final String pyBaseCmd = isUnix() ? "python${cfg.python}" : 'python'
	final String pyCmd = envPath + (isUnix() ? '/bin/python' : '/Scripts/python')

	psl.runCmd("${pyBaseCmd} -m venv ${envPath}")

	// some packages (like 'arcgis') require up-to-date pip, let's upgrade ourselves
	psl.runCmd("${pyCmd} -m pip install --upgrade pip")

	// 'arcgis' will use legacy installation method if 'wheel' is not installed beforehand
	psl.runCmd("${pyCmd} -m pip install wheel")

	psl.runCmd("${pyCmd} -m pip install -r requirements.txt")

	return pyCmd
}
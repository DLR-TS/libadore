# This Makefile contains useful targets that can be included in downstream projects.

ifeq ($(filter libadore.mk, $(notdir $(MAKEFILE_LIST))), libadore.mk)

MAKEFLAGS += --no-print-directory

.EXPORT_ALL_VARIABLES:
LIBADORE_PROJECT=libadore

LIBADORE_MAKEFILE_PATH:=$(shell realpath "$(shell dirname "$(lastword $(MAKEFILE_LIST))")")
ifeq ($(SUBMODULES_PATH),)
    LIBADORE_SUBMODULES_PATH:=${LIBADORE_MAKEFILE_PATH}
else
    LIBADORE_SUBMODULES_PATH:=$(shell realpath ${SUBMODULES_PATH})
endif
MAKE_GADGETS_PATH:=${LIBADORE_SUBMODULES_PATH}/make_gadgets
ifeq ($(wildcard $(MAKE_GADGETS_PATH)/*),)
    $(info INFO: To clone submodules use: 'git submodules update --init --recursive')
    $(info INFO: To specify alternative path for submodules use: SUBMODULES_PATH="<path to submodules>" make build')
    $(info INFO: Default submodule path is: ${LIBADORE_MAKEFILE_PATH}')
    $(error "ERROR: ${MAKE_GADGETS_PATH} does not exist. Did you clone the submodules?")
endif
REPO_DIRECTORY:=${LIBADORE_MAKEFILE_PATH}

LIBADORE_TAG:=$(shell cd ${MAKE_GADGETS_PATH} && make get_sanitized_branch_name REPO_DIRECTORY=${REPO_DIRECTORY})
LIBADORE_IMAGE=${LIBADORE_PROJECT}:${LIBADORE_TAG}

LIBADORE_CMAKE_BUILD_PATH="${LIBADORE_PROJECT}/build"
LIBADORE_CMAKE_INSTALL_PATH="${LIBADORE_CMAKE_BUILD_PATH}/install"

DOCKER_IMAGE_EXCLUSION_LIST?="alpine:** csaps-cpp:latest python:** ubuntu:**"
DOCKER_IMAGE_INCLUSION_LIST?=""

include ${MAKE_GADGETS_PATH}/make_gadgets.mk
include ${MAKE_GADGETS_PATH}/docker/docker-tools.mk
include ${MAKE_GADGETS_PATH}/docker/docker-image-cacher.mk

include ${LIBADORE_SUBMODULES_PATH}/cppcheck_docker/cppcheck_docker.mk
include ${LIBADORE_SUBMODULES_PATH}/cpplint_docker/cpplint_docker.mk
include ${LIBADORE_SUBMODULES_PATH}/lizard_docker/lizard_docker.mk

CPP_PROJECT_DIRECTORY:=${LIBADORE_MAKEFILE_PATH}/${LIBADORE_PROJECT}


.PHONY: build_libadore 
build_libadore: ## Build libadore
	cd "${LIBADORE_MAKEFILE_PATH}" && make

.PHONY: test_libadore 
test_libadore: ## run libadore unit tests 
	cd "${LIBADORE_MAKEFILE_PATH}" && make test

.PHONY: clean_libadore
clean_libadore: ## Clean libadore build artifacts
	cd "${LIBADORE_MAKEFILE_PATH}" && make clean

.PHONY: branch_libadore
branch_libadore: ## Returns the current docker safe/sanitized branch for libadore
	@printf "%s\n" ${LIBADORE_TAG}

.PHONY: image_libadore
image_libadore: ## Returns the current docker image name for libadore
	@printf "%s\n" ${LIBADORE_IMAGE}

.PHONY: update_libadore
update_libadore:
	cd "${LIBADORE_MAKEFILE_PATH}" && git pull

endif

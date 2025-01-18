# encoding: utf-8

import subprocess
from dataclasses import dataclass
import re
import sys
import os

# structs


@dataclass
class TagCommit:
    """Class for storing a tag and its associated commit"""
    tag: str
    commit: str

    @staticmethod
    def init_from_str(git_show_ref_str: str):
        sha = re.findall(r"^[\w]*", git_show_ref_str)
        tag = re.findall(r"(?<= refs/tags/)[\w\.]*", git_show_ref_str)
        return TagCommit(tag[0] if len(tag) else "", sha[0] if len(sha) else "")

    def version_to_numeric(self):
        if not is_tag_version(self.tag):
            return None
        str_list = self.tag[1:].split('.')
        return int(str_list[0]), int(str_list[1]), int(str_list[2])


def find_tag_for_commit(tag_commit_list: list[TagCommit], commit: str) -> int:
    """Return index of the TagCommit found or -1 if not found"""
    i = 0
    for c in tag_commit_list:
        if commit == c.commit:
            return i
        i += 1
    return -1


def find_commit_for_tag(tag_commit_list: list[TagCommit], tag: str) -> int:
    """Return index of the TagCommit found or -1 if not found"""
    i = 0
    for c in tag_commit_list:
        if tag == c.tag:
            return i
        i += 1
    return -1


def find_versions(tag_commit_list: list[TagCommit]) -> list[TagCommit]:
    out = []
    for c in tag_commit_list:
        if is_tag_version(c.tag):
            out.append(c)
    return out

# utils


def run(cmd_arg_list: list) -> str:
    proc = subprocess.Popen(cmd_arg_list,
                            stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            universal_newlines=True,
                            bufsize=0)
    return "".join(proc.stdout).rstrip()

# basics


def get_tag_commit_list() -> list[TagCommit]:
    """Index 0 is the most recent"""
    commit_tags = run(["git", "show-ref",
                      "--tags", "--abbrev"]).splitlines()
    out = []
    for c in commit_tags:
        tmp = TagCommit.init_from_str(c)
        out.append(tmp)
    return out


def get_tags() -> list[str]:
    tag_list = run(["git", "tag"])
    return tag_list.splitlines()


def is_tag_version(tag: str) -> bool:
    version = re.findall(r"^v\d+\.\d+\.\d+$", tag)
    return len(version) > 0

# actual functions


def is_in_modified_state() -> bool:
    return len(run(["git", "diff"])) > 1


def is_current_commit_release() -> bool:
    current_tag = get_current_tag()
    return is_tag_version(
        current_tag) if current_tag else False


def get_current_commit() -> str:
    return run(["git", "rev-parse", "--short", "HEAD"])


def get_current_tag() -> str:
    tag_commits = get_tag_commit_list()
    tag_index = find_tag_for_commit(tag_commits, get_current_commit())
    if tag_index < 0:
        return None
    return tag_commits[tag_index].tag


def get_latest_version_on_head() -> str:
    tag_commits = get_tag_commit_list()
    for t in tag_commits:
        if is_tag_version(t.tag):
            return t.tag


def replace_in_template(input_template_path: str, output_path: str):
    def to_cpp_bool(val: bool):
        return "true" if val else "false"

    tag_commit_list = get_tag_commit_list()
    version_list = find_versions(tag_commit_list)
    print(f"VersionList: {version_list}")
    latest_ver_maj, latest_ver_min, latest_ver_patch = version_list[-1].version_to_numeric(
    )
    current_tag = get_current_tag()

    known_macros = {r"{py_major}": latest_ver_maj,
                    r"{py_minor}": latest_ver_min,
                    r"{py_patch}": latest_ver_patch,
                    r'{py_is_release}': to_cpp_bool(is_current_commit_release()),
                    r'{py_is_modified}': to_cpp_bool(is_in_modified_state()),
                    r'{py_current_commit}': get_current_commit(),
                    r'{py_current_tag}': current_tag if current_tag else ""}

    # sep
    # sep
    # sep
    lines = None
    try:
        with open(input_template_path) as f:
            lines = f.read()
    except Exception as e:
        print(f"Error when opening: \"{input_template_path}\": {e}")
        return False

    for macro, value in known_macros.items():
        lines = lines.replace(macro, str(value))

    try:
        # check if the file already exists:
        if os.path.exists(output_path):
            with open(output_path, 'r') as f:
                if f.read() == lines:
                    return True
        with open(output_path, 'w') as f:
            f.write(lines)
    except Exception as e:
        print(f"Error when opening: \"{output_path}\": {e}")
        return False

    return True


def main(argv):
    print("modified?", is_in_modified_state())
    print("is release?", is_current_commit_release())
    print("current commit:", get_current_commit())
    print("current_tag:", get_current_tag())
    tag_commit_list = get_tag_commit_list()
    print("tags commits list:", tag_commit_list)
    print("-------------------")

    print("Argv:", argv)
    if len(argv) != 3:
        print("Usage: generate_cpp.py [template_file] [output_file]")
        return 1

    template = argv[1]
    output = argv[2]
    print(f"Generate [{output}] based on [{template}]")
    success = replace_in_template(argv[1], argv[2])
    print("Cpp generation successful?", success)

    exit(0 if success else 1)


if __name__ == "__main__":
    exit(main(sys.argv))

"""Generate the library catalog embedded in README.md."""

from __future__ import annotations

import os
from pathlib import Path
from urllib.parse import quote

ROOT = Path(__file__).resolve().parents[2]
README = ROOT / "README.md"
START_MARKER = "<!-- FOLDER-TOC -->"
END_MARKER = "<!-- /FOLDER-TOC -->"

FILE_TYPES = {
    "intlib": ("Integrated libraries", ".IntLib", "🔗"),
    "schlib": ("Symbol libraries", ".SchLib", "📐"),
    "pcblib": ("Footprint libraries", ".PcbLib", "🦶"),
    "step": ("3D models", ".STP, .STEP", "🎯"),
    "pdf": ("Datasheets", ".pdf", "📄"),
}
DETAIL_ORDER = ("pdf", "schlib", "pcblib", "step", "intlib")
FOLDER_LABELS = {
    "intlib": "IntLib",
    "schlib": "SchLib",
    "pcblib": "PcbLib",
    "step": "3D",
    "pdf": "PDF",
}


def file_type(path: Path) -> str | None:
    """Return the catalog type for a supported file."""
    suffix = path.suffix.lower()
    if suffix in {".stp", ".step"}:
        return "step"
    candidate = suffix.removeprefix(".")
    return candidate if candidate in FILE_TYPES else None


def encoded_path(path: Path) -> str:
    """Encode each path segment without escaping separators."""
    return "/".join(quote(part) for part in path.parts)


def scan_repository() -> tuple[list[Path], dict[str, dict[Path, list[Path]]]]:
    """Collect visible directories and supported files."""
    directories: list[Path] = []
    files_by_type: dict[str, dict[Path, list[Path]]] = {
        key: {} for key in FILE_TYPES
    }

    for current, child_dirs, files in os.walk(ROOT):
        current_path = Path(current)
        child_dirs[:] = sorted(
            (
                name
                for name in child_dirs
                if not name.startswith(".")
            ),
            key=str.casefold,
        )

        if current_path != ROOT:
            directories.append(current_path.relative_to(ROOT))

        relative_directory = current_path.relative_to(ROOT)
        for name in sorted(files, key=str.casefold):
            kind = file_type(Path(name))
            if kind is None:
                continue
            files_by_type[kind].setdefault(relative_directory, []).append(Path(name))

    return directories, files_by_type


def repository_slug() -> str:
    """Return the owner/repository slug supplied by GitHub Actions."""
    return os.environ.get(
        "GITHUB_REPOSITORY", "BingL-Li/Altium_Suppllier-Raw_Lib"
    )


def badges(counts: dict[str, int]) -> list[str]:
    """Build repository and catalog badges."""
    slug = repository_slug()
    library_count = counts["intlib"] + counts["schlib"] + counts["pcblib"]
    return [
        (
            "![Altium Designer]"
            "(https://img.shields.io/badge/Altium%20Designer-Compatible-blue"
            "?style=flat-square&logo=altiumdesigner) "
            "![License: CC BY-NC-SA 4.0]"
            "(https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0"
            "-lightgrey.svg?style=flat-square&logo=creativecommons)"
        ),
        (
            f"![GitHub repo size](https://img.shields.io/github/repo-size/{slug}"
            "?style=flat-square&logo=github) "
            f"![GitHub last commit](https://img.shields.io/github/last-commit/{slug}"
            "?style=flat-square&logo=github) "
            f"![GitHub stars](https://img.shields.io/github/stars/{slug}"
            "?style=flat-square&logo=github)"
        ),
        (
            f"![Datasheets](https://img.shields.io/badge/Datasheets-{counts['pdf']}"
            "-yellow?style=flat-square&logo=filedotio) "
            f"![Total Libraries](https://img.shields.io/badge/Libraries-{library_count}"
            "-blue?style=flat-square&logo=opensourcehardware) "
            f"![IntLib](https://img.shields.io/badge/IntLib-{counts['intlib']}"
            "-green?style=flat-square&logo=librariesdotio) "
            f"![SchLib](https://img.shields.io/badge/SchLib-{counts['schlib']}"
            "-orange?style=flat-square&logo=electron) "
            f"![PcbLib](https://img.shields.io/badge/PcbLib-{counts['pcblib']}"
            "-red?style=flat-square&logo=circuitverse) "
            f"![3D Models](https://img.shields.io/badge/3D%20Models-{counts['step']}"
            "-purple?style=flat-square&logo=blender)"
        ),
    ]


def directory_counts(
    directory: Path, files_by_type: dict[str, dict[Path, list[Path]]]
) -> dict[str, int]:
    """Count supported files directly inside a directory."""
    return {
        kind: len(grouped_files.get(directory, []))
        for kind, grouped_files in files_by_type.items()
    }


def format_size(path: Path) -> str:
    """Format a file size for the 3D model listing."""
    size = path.stat().st_size
    if size > 1024 * 1024:
        return f"{size / (1024 * 1024):.1f} MB"
    if size > 1024:
        return f"{size / 1024:.1f} KB"
    return f"{size} B"


def generate_catalog() -> str:
    """Generate the Markdown catalog."""
    directories, files_by_type = scan_repository()
    counts = {
        kind: sum(len(files) for files in grouped_files.values())
        for kind, grouped_files in files_by_type.items()
    }
    lines: list[str] = []

    for badge_row in badges(counts):
        lines.extend((badge_row, ""))

    lines.extend(
        (
            "## Library catalog",
            "",
            "### Summary",
            "",
            "| Resource | File type | Count |",
            "| --- | --- | ---: |",
        )
    )
    for kind in FILE_TYPES:
        label, extension, _ = FILE_TYPES[kind]
        lines.append(f"| {label} | `{extension}` | {counts[kind]} |")

    lines.extend(
        (
            "",
            "### Browse by folder",
            "",
            "<details>",
            "<summary>Show the complete folder structure</summary>",
            "",
        )
    )
    for directory in sorted(directories, key=lambda path: str(path).casefold()):
        counts_here = directory_counts(directory, files_by_type)
        descriptions = [
            f"{FILE_TYPES[kind][2]} {count} {FOLDER_LABELS[kind]}"
            for kind, count in counts_here.items()
            if count
        ]
        indent = "  " * (len(directory.parts) - 1)
        link = f"./{encoded_path(directory)}/"
        description = f" — *{' | '.join(descriptions)}*" if descriptions else ""
        lines.append(
            f"{indent}- [📂 **{directory.name}**]({link}){description}"
            if descriptions
            else f"{indent}- [📂 {directory.name}]({link})"
        )

    lines.extend(("", "</details>", "", "### Browse by file type", ""))
    for kind in DETAIL_ORDER:
        grouped_files = files_by_type[kind]
        if not grouped_files:
            continue
        label, extension, icon = FILE_TYPES[kind]
        lines.extend(("<details>", f"<summary>{icon} {label} ({extension})</summary>", ""))
        for directory in sorted(grouped_files, key=lambda path: str(path).casefold()):
            lines.extend((f"#### 📂 {directory.name}", ""))
            for filename in grouped_files[directory]:
                relative_path = directory / filename
                size = (
                    f" *({format_size(ROOT / relative_path)})*"
                    if kind == "step"
                    else ""
                )
                lines.append(
                    f"  - [{filename}]({encoded_path(relative_path)}){size}"
                )
            lines.append("")
        lines.extend(("</details>", ""))

    return "\n".join(lines).rstrip() + "\n"


def update_readme(catalog: str) -> None:
    """Replace the generated section while preserving hand-written content."""
    content = README.read_text(encoding="utf-8")
    if content.count(START_MARKER) != 1 or content.count(END_MARKER) != 1:
        raise ValueError("README must contain exactly one catalog marker pair")

    before, remainder = content.split(START_MARKER, 1)
    _, after = remainder.split(END_MARKER, 1)
    README.write_text(
        f"{before}{START_MARKER}\n{catalog}{END_MARKER}{after}",
        encoding="utf-8",
    )


if __name__ == "__main__":
    update_readme(generate_catalog())

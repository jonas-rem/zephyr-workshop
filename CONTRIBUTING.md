# Contributing

Contributions are welcome! If you have questions, ideas for additional content,
or want to contribute, please open an Issue or Pull Request in the GitHub
Repository.

## Development

### Workshop Releases

Workshop-specific releases use branches named `<city-MM.YY>` and tags named
`vX.Y.Z-<city-MM.YY>`, for example `vienna-06.26` and
`v0.9.0-vienna-06.26`.

Pushing a release tag, for example `git push origin v0.9.0-vienna-06.26`,
triggers CI to create a GitHub Release with docs/slides PDF and HTML artifacts.

### Building the Documentation

The documentation is located in the `doc` folder. Install
[uv](https://docs.astral.sh/uv/) and Node.js, then build from the repository
root with `make`:

```shell
# Build integrated documentation and slides
make docs
# Or for live preview with autobuild
make html
```

### Building Slides Standalone

The presentation slides are located in `doc/slides` and are written with
[Slidev](https://sli.dev/). They are automatically built and included when
building the documentation. For live slide editing, use the Slidev development
server:

```shell
make slides
```

The standalone build output is located in
`doc/slides_dist/zephyr-workshop_slides`.

## Licensing

This repository uses two different licenses:

- **Code and Documentation**: Licensed under the Apache License 2.0
  (see `LICENSE` file).
- **Presentation Slides**: The content in `doc/slides` is licensed under the
  Creative Commons Attribution-ShareAlike 4.0 International License (CC BY-SA
  4.0). See `LICENSE_SLIDES` file for the full license text.

By contributing to this project, you agree that your contributions will be
licensed under the respective license of the component you are contributing to.

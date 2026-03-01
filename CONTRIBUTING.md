# Contributing

Contributions are welcome! If you have questions, ideas for additional content,
or want to contribute, please open an Issue or Pull Request in the GitHub
Repository.

## Development

### Building the Documentation

The documentation is located in the `doc` folder. To build the full
documentation including the integrated slides, use `tox`:

```shell
cd doc
# Build integrated HTML documentation (Sphinx + Slides)
tox -e html
# Or for live preview with autobuild
tox -e docs
```

### Building Slides Standalone

The presentation slides are located in `doc/slides` and are written with
[Slidev](https://sli.dev/). They are automatically built and included when
building the documentation, but they can also be built standalone:

```shell
cd doc
tox -e slides
```

The standalone build output is located in
`doc/slides_dist/zephyr-workshop_slides`.

#### Live Preview
For the best experience when editing slides, use the Slidev development server
which provides instant hot-reloading:

```shell
cd doc/slides
npm install
npm run dev
```

## Licensing

This repository uses two different licenses:

- **Code and Documentation**: Licensed under the Apache License 2.0
  (see `LICENSE` file).
- **Presentation Slides**: The content in `doc/slides` is licensed under the
  Creative Commons Attribution-ShareAlike 4.0 International License (CC BY-SA
  4.0). See `LICENSE_SLIDES` file for the full license text.

By contributing to this project, you agree that your contributions will be
licensed under the respective license of the component you are contributing to.

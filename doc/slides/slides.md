---
theme: academic
colorSchema: light
layout: cover
coverAuthor: Jonas Remmert
coverAuthorUrl: https://github.com/jonas-rem
coverDate: ""
title: Hands-on Zephyr Project Workshop
routerMode: hash
info: |
  ## Hands-on Zephyr Project Workshop
  Navigating Low Power IoT Development with Practical Examples

  [github.com/jonas-rem/zephyr-workshop](https://github.com/jonas-rem/zephyr-workshop)
keywords: zephyr,rtos,embedded,iot
exportFilename: zephyr-workshop-slides
download: false
lineNumbers: false
drawings:
  persist: false
transition: none
mdc: true
hideInToc: true
fonts:
  sans: Fira Sans
  mono: Fira Code
themeConfig:
  paginationPages: false
---

<style>
/* All slides need position relative for logo positioning */
.slidev-layout {
  position: relative !important;
}

/* Light logo for cover slide (white background) - upper right */
.slidev-layout.cover::after {
  content: '';
  position: absolute;
  top: 8px;
  right: 30px;
  width: 120px;
  height: 50px;
  background-image: url('/images/smight-logo-light.png');
  background-repeat: no-repeat;
  background-position: center;
  background-size: contain;
  z-index: 100;
  pointer-events: none;
}

/* Embedded World logo for cover slide - upper left */
.slidev-layout.cover::before {
  content: '';
  position: absolute;
  top: 8px;
  left: 30px;
  width: 180px;
  height: 50px;
  background-image: url('/images/embedded-world-logo.png');
  background-repeat: no-repeat;
  background-position: center;
  background-size: contain;
  z-index: 100;
  pointer-events: none;
}

/* Dark logo for content slides (grey header bar) */
.slidev-layout:not(.cover):not(.section):not(.intro)::after {
  content: '';
  position: absolute;
  top: 8px;
  right: 30px;
  width: 120px;
  height: 50px;
  background-image: url('/images/smight-logo.png');
  background-repeat: no-repeat;
  background-position: center;
  background-size: contain;
  z-index: 100;
  pointer-events: none;
}

/* Footer for all slides */
.slidev-layout .slidev-layout-footer {
  position: absolute;
  bottom: 12px;
  right: 30px;
  font-size: 0.65rem;
  color: #3b3b3b;
  z-index: 100;
  pointer-events: none;
}

.slidev-layout .slidev-layout-footer::after {
  content: 'Zephyr Workshop, 2026';
}
</style>

# Hands-on Zephyr Project Workshop

## Navigating Low Power IoT Development with Practical Examples

---
src: ./pages/00-toc.md
---

---
src: ./pages/01-introduction.md
---

---
src: ./pages/02-development-setup.md
---

---
src: ./pages/03-workspace-application.md
---

---
src: ./pages/04-code-examples.md
---

---
src: ./pages/05-application-development.md
---

---
src: ./pages/06-summary.md
---

---
src: ./pages/07-backup-slides.md
hideInToc: true
---

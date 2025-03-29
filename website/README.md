# Website directory

The following directory contains all website files. The website uses popular framework [Hugo](https://gohugo.io/).

---

## Requirements

In order to be able to work with Hugo you need `git` and `hugo`. I will introduce how to install them both for Linux Debian systems. However, I will also share links if you use other OSs.

### Install `git`.

- For other OSs, rather than Linux Debian, please check this [link](https://git-scm.com/downloads)
- Run the usual update commands:

```bash
sudo apt-get update && sudo apt-get -y upgrade
```

- Install `git`:

```bash
sudo apt-get install -y git
```

- Check installation is successful by running the following code snippet:

```bash
git --version
```

- You should get an output similar to the following one:

```bash
git version 2.34.1
```

### Install `hugo`.

- Again, please refer to this [link](https://gohugo.io/installation/) to install `hugo` in systems other than Linux Debian.

- To install the latest version, it is best to use `snap` package manager (instead of in-built `apt`). To do so, first install the package manager:

```bash
sudo apt-get update && sudo apt-get -y upgrade
sudo apt-get install -y snap
```

- To check for successful installation, run:

```bash
snap --version
```

- You should see an output similar to the following:

```bash
snap    2.67.1+22.04
snapd   2.67.1+22.04
series  16
ubuntu  22.04
kernel  5.15.167.4-microsoft-standard-WSL2
```

- Install `hugo` by using `snap`:

```bash
sudo snap install hugo
```

- Confirm successful installation by running:

```bash
hugo version
```

- The output should be like this:

```bash
hugo v0.145.0-666444f0a52132f9fec9f71cf25b441cc6a4f355+extended linux/amd64 BuildDate=2025-02-26T15:41:25Z VendorInfo=snap:0.145.0
```

---

## Website structure

Now that you have the software working, let's take some time to understand the project structure. The structure of the `website` directory is as follows:

```bash
website/
    ├── hugo.production.yaml
    ├── hugo.yaml
    ├── .hugo_build.lock
    ├── archetypes/
    ├── assets/
    │   ├── images/
    │   ├── js/
    │   └── sass/
    ├── content/
    ├── layouts/
    ├── php/
    ├── public/
    ├── resources/
    ├── static/
    └── themes/
```

### Hugo configuration files

This file (`hugo.yaml` or `hugo.production.yaml`) define the global settings for Hugo static site generator. This file is located in the root directory of the Hugo project and controls various aspects of the site generation:

- Website metadata (title, baseURL, language, etc.)
- Theme settings
- Content rendering options
- Build settings (caching, output formats, minification, etc.)
- Permalinks and URL structures
- Menu and navigation settings
- Third-party integrations (Google Analytics, Disqus, etc.)

> [!NOTE] > **Why do we use two configuration files?**
> In our case we use `hugo.yaml` when working locally on the website, so that file structure works properly.
> However, since we are deploying our site to [GitHub Pages](https://pages.github.com/), and the root directory is different there, we use `hugo.production.yaml` for preserving the structure.

### Archetypes directory

This directory contains blueprints (templates) for new pages/posts. It defines a default front matter for content types. For example:

```markdown
+++
date = '{{ .Date }}'
draft = true
title = '{{ replace .File.ContentBaseName "-" " " | title }}'
+++
```

Running `hugo new content/blog/new-post.md` will use the default archetype.

### Assets directory

Contains raw assets (SASS/SCSS, JavaScript, images) that Hugo processes with **Pipelines** (e.g., minification, fingerprinting).

- `images/` &rarr; Raw images used in the site.
- `js/` &rarr; JavaScript files (bundled via Hugo Pipes).
- `sass/` &rarr; Stylesheets, usually compiled into CSS.

### Content directory

Contains the actual Markdown content for the website.

- Organized into the following sections:
  - `content/blog`: directory for storing blog posts.
  - `content/products`: directory for storing products.
- Hugo converts this Markdown files into HTML.

### Layouts directory

Defines the templates and HTML structure for the website.

- Overrides theme templates if a theme is used.
- Contains in our case HTML templates for the products page.

### Php directory

Contains all the backend PHP processing required for the contact form of the website.

### Public directory

This is the **output directory** where site is built after running `hugo`. It contains the final static site (HTML, CSS, JS, images).

- This is what gets deployed into GitHub Pages.

### Resources directory

It stores processed assets from Hugo Pipes (e.g., minified CSS, optimized images).

- **Cached built artifacts**: if deleted, Hugo will regenerate them.

### Static directory

It is where static files (images, JavaScript, CSS) are stored, and **will not be processed** by Hugo.

- Anything stored here is copied **as-is** to `public/`.

### Themes directory

Contains installed Hugo themes, in our case [Hugo Zen](https://github.com/frjo/hugo-theme-zen).

- Each theme has its own `layouts/`, `static/` and `archetypes/`.

---

## Website editing workflow.

- For simple content editing you can work on the Markdown files inside `content/`.

- You can always refer to the [Zen theme repository](https://github.com/frjo/hugo-theme-zen) and [demo site](https://github.com/frjo/zen-demo) for inspiration.

- It is also interesting to check [Hugo settings page](https://gohugo.io/configuration/all/).

- You can build the site locally to test the changes you are applying by running the following command in the root website directory:

```bash
hugo server -D
```

- You should get an output similar to this one:

```bash
Watching for changes in /home/lucash/NTUST_GIMT/2025_Spring_Semester/cloud-and-fog-computing-in-the-iot/cf-iot-spring-25-project/website/{archetypes,assets,content,data,i18n,layouts,static,themes}
Watching for config changes in /home/lucash/NTUST_GIMT/2025_Spring_Semester/cloud-and-fog-computing-in-the-iot/cf-iot-spring-25-project/website/hugo.yaml, /home/lucash/NTUST_GIMT/2025_Spring_Semester/cloud-and-fog-computing-in-the-iot/cf-iot-spring-25-project/website/themes/zen/hugo.yaml
Start building sites …
hugo v0.145.0-666444f0a52132f9fec9f71cf25b441cc6a4f355+extended linux/amd64 BuildDate=2025-02-26T15:41:25Z VendorInfo=snap:0.145.0


                   | EN
-------------------+-----
  Pages            | 47
  Paginator pages  |  0
  Non-page files   |  0
  Static files     | 12
  Processed images |  0
  Aliases          | 17
  Cleaned          |  0

Built in 145 ms
Environment: "development"
Serving pages from disk
Running in Fast Render Mode. For full rebuilds on change: hugo server --disableFastRender
Web Server is available at http://localhost:1313/website/ (bind address 127.0.0.1)
Press Ctrl+C to stop

```

- If you click on the localhost link you should see the changes you applied on the website. You can edit and see changes on real time.

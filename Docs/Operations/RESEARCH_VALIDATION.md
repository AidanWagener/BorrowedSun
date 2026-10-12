# Research delivery validation — 12 October 2026

The development handbook and additional repository sprint are complete as research deliverables. This record supplements [engine and tooling validation](VALIDATION.md); research acceptance does not imply production qualification of any candidate.

## Deliverables checked

- Development handbook: 42 PDF pages, 41 sections, 113 source records representing 110 distinct URLs, and 38 asset candidates. Every PDF page was visually inspected. Offline HTML search, empty state, clearing and section navigation were exercised in the browser.
- Framework report: 33 PDF pages, 48 sections and 155 distinct linked primary source URLs. Every final page was visually inspected; a single-line final-page orphan was repaired before delivery. The browser showed 0 of 48 sections for a nonsense query, 7 of 48 for FlowGraph, and 48 of 48 after clearing. The experiment-queue link navigated to section 45 and rendered correctly.
- The repository register preserves 41 assessments consolidated into 40 unique candidates, including a native engine component and the official Lyra sample. Canonical URLs, inspected revisions, license evidence, engine claims, limitations and decisions are recorded where available. The duplicated Allar style-guide assessment is preserved under additionalAssessments.
- Six proposed experiment records point to the framework report and require the existing recovery and first-map backlog milestones. They remain proposed, with one active experiment at a time.

## Scope boundaries

No third-party repository from the additional research sprint was installed, built as part of the game, or executed as an editor plugin. Installed native MCP source and descriptors were inspected without enabling the plugin or starting a server. Upstream declarations and release claims are distinguished from local tests. Exact inspected default-branch revisions are not automatically recommended installation pins.

The additional sprint changed documentation and registers only. It did not alter the already tested game source or operational scripts, so the previously recorded engine and regression results remain the relevant implementation evidence. Recovery on independent storage, asset acquisition/import, first-map creation, cook/package/launch and gameplay qualification remain future work.

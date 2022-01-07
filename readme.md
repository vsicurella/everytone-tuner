# Everytone Tuner

**Current version**: alpha

Everytone Tuner is a free [JUCE](https://juce.com/)-based tool for remapping and retuning polyphonic MIDI data via pitchbend messages, primarily for use with synthesizers that support [MPE](https://www.midi.org/midi-articles/midi-polyphonic-expression-mpe).

## Features

The following features are currently available but are subject to change while in alpha.

- Cross-platform VST3 and standalone app formats
- Load SCL and TUN files
- Create equal temperament tunings
- Automatic multichannel MIDI mapping (linear and channel-based periodic)
- MPE upscaling, with lower/upper zone options
- Legacy omnichannel support (use all 16 channels without global voice or controller messages)
- Custom polyphony limit (up to 15 with MPE or 16 with legacy omnichannel mode)
- Voice allocation options
- Edit pitchbend range
- Pitchbend message update options (once-per-note or a stream while note is on)

### Planned

- More plugin formats
- Decoupling of *tuning reference* and *mapping root* (internally supported, needs UI)
- MTS-ESP master & client modes
- More scale-creation methods (inc. Rank-N, harmonic subsets)
- Support more interval input types than cents, and display the exact lines from loaded SCL files
- KBM & custom mapping methods
- Saving scale and mappings to a file
- Dynamic tuning through controller mappings or modulations
- More voice/pitch allocation options (ex. increase polyphony limit by sharing a channel if pitchbend is the same)
- More tuning visualization options (inc. many more tone wheel improvements)
- Edit source tuning for something else than 12-edo @ A 440hz = MIDI Note 69 (internally supported, needs UI)
- CV Output
- Prettier UI (custom skins would be so awesome)

## Limitations

For those unfamiliar with MIDI 1.0 limitations, pitchbend messages affect all active notes because they are applied per-channel, and in most modern cases only one channel is used at a time. The MPE specification officialized the usage of more than one MIDI channel to make pitchbend and other channel-based messages apply to (optionally) a single note along with one reserved channel that can still create global messages. This specification fortunately popularized this multichannel-pitchbend tuning method. Before MPE, some synths, particularly multitimbral ones, would work with this method, but their implementations varied. For the best results with Everytone Tuner, an MPE synth is recommended, but a legacy mode (excluding a reserved global channel) is also included.

Certain DAWs introduce other MIDI routing difficulties, such as Ableton Live, that require a wrapper-plugin that can host both a tuner and a synth inside of it, so the MIDI data is not mutated between them. I plan to offer some more detailed suggestions to these particular issues in the future.

## Known Issues

- TUN files are only loaded as a frequency table, which has some limitations, with which the tone circle visualization doesn't display as well.

## Credits

Huge thanks to the all the developers, sharers, and maintainers of microtonal knowledge across various online resources such as members of the Xenharmonic Alliance ([Facebook](https://www.facebook.com/groups/476404232379884), [Discord](https://discord.com/invite/FSF5JFT)), the [Microtonal Music and Tuning Theory](https://www.facebook.com/groups/497105067092502) group on Facebook, and the [XenWiki](https://xen.wiki/). I appreciate the time many of them spend in presenting information, working with individuals to help them understand concepts, reducing confusion, and keeping misinformation in check.

I also want to give a shoutout to Sevish and Lajos Mészáros, maintainers of [ScaleWorkshop](https://github.com/SeanArchibald/scale-workshop) not only for providing an excellent accessible online tool, but also being helpful in my collaboration with ScaleWorkshop, which helped inform some of the designs within this application.

## Licenses

The current SCL & TUN reading functionality is provided by the [Anamark-Tuning-Library](https://github.com/zardini123/AnaMark-Tuning-Library) included in this repository, with slight modifications to access the SCL filename and description. The library is under an MIT License as follows:

> Copyright (C) 2009 Mark Henning, Germany, http://www.mark-henning.de
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
>
***

The rest of the code is under Unlicense as follows:

> This is free and unencumbered software released into the public domain.
>
> Anyone is free to copy, modify, publish, use, compile, sell, or
> distribute this software, either in source code form or as a compiled
> binary, for any purpose, commercial or non-commercial, and by any
> means.
>
> In jurisdictions that recognize copyright laws, the author or authors
> of this software dedicate any and all copyright interest in the
> software to the public domain. We make this dedication for the benefit
> of the public at large and to the detriment of our heirs and
> successors. We intend this dedication to be an overt act of
> relinquishment in perpetuity of all present and future rights to this
> software under copyright law.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
> EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
> MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
> IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
> OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
> ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
> OTHER DEALINGS IN THE SOFTWARE.
>
> For more information, please refer to <http://unlicense.org/>

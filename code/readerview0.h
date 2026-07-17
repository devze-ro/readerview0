#ifndef READERVIEW0_H
#define READERVIEW0_H

#include "readerview0_version.h"
#include "ui0.h"

#if !defined(UI0_API_VERSION) || \
    UI0_API_VERSION != READERVIEW0_UI0_REQUIRED_API_VERSION
#error "readerview0 API 3 requires UI0 API 91"
#endif

#include "reader_view/reader_view.h"

#endif /* READERVIEW0_H */

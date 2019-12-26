#pragma once

namespace incr4k
{
enum class rc
{
    ok = 0,
    failed,
    file_error,
	not_a_dir,
	same,
	less,
	greater,
	different
};
}

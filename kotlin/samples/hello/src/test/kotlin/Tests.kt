package sample

import kotlin.test.assertEquals
import org.junit.Test

class TestSource {
    @Test fun f() {
        assertEquals("Hello, world!", getGreeting())
    }
}

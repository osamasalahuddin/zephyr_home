#include <app/drivers/blink.h>


static inline int z_impl_blink_set_period_ms(const struct device *dev,
    unsigned int period_ms)
{
__ASSERT_NO_MSG(DEVICE_API_IS(blink, dev));

return DEVICE_API_GET(blink, dev)->set_period_ms(dev, period_ms);
}

#include <syscalls/blink.c>

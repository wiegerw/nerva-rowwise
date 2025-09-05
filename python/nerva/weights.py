# Copyright 2022 Wieger Wesselink.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or http://www.boost.org/LICENSE_1_0.txt)

class WeightInitializer(object):
    pass


class HeNormal(WeightInitializer):
    def __str__(self):
        return 'HeNormal'


class HeUniform(WeightInitializer):
    def __str__(self):
        return 'HeUniform'


class XavierNormal(WeightInitializer):
    def __str__(self):
        return 'XavierNormal'


class XavierUniform(WeightInitializer):
    def __str__(self):
        return 'XavierUniform'


class Normal(WeightInitializer):
    def __str__(self):
        return 'Normal'


class Uniform(WeightInitializer):
    def __str__(self):
        return 'Uniform'


class Zero(WeightInitializer):
    def __str__(self):
        return 'Zero'


def parse_weight_initializer(text: str):
    if text == 'HeNormal':
       return HeNormal()
    elif text == 'HeUniform':
        return HeUniform()
    elif text == 'XavierNormal':
        return XavierNormal()
    elif text == 'XavierUniform':
        return XavierUniform()
    elif text == 'Normal':
        return Normal()
    elif text == 'Uniform':
        return Uniform()
    elif text == 'Zero':
        return Zero()
    raise RuntimeError(f"unknown weight initializer '{text}'")

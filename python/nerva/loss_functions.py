# Copyright 2022 - 2023 Wieger Wesselink.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or http://www.boost.org/LICENSE_1_0.txt)

import nervalibrowwise

class LossFunction(nervalibrowwise.loss_function):
    def __str__(self):
        return 'LossFunction()'


class SquaredErrorLoss(nervalibrowwise.squared_error_loss):
    def __str__(self):
        return 'SquaredErrorLoss()'


class CrossEntropyLoss(nervalibrowwise.cross_entropy_loss):
    def __str__(self):
        return 'CrossEntropyLoss()'


class LogisticCrossEntropyLoss(nervalibrowwise.logistic_cross_entropy_loss):
    def __str__(self):
        return 'LogisticCrossEntropyLoss()'


class NegativeLogLikelihoodLoss(nervalibrowwise.negative_log_likelihood_loss):
    def __str__(self):
        return 'NegativeLogLikelihoodLoss()'


# tag::softmax_cross_entropy_loss[]
class SoftmaxCrossEntropyLoss(nervalibrowwise.softmax_cross_entropy_loss):
    def __str__(self):
        return 'SoftmaxCrossEntropyLoss()'
# end::softmax_cross_entropy_loss[]


def parse_loss_function(text: str) -> LossFunction:
    if text == "SquaredError":
        return SquaredErrorLoss()
    elif text == "CrossEntropy":
        return CrossEntropyLoss()
    elif text == "LogisticCrossEntropy":
        return LogisticCrossEntropyLoss()
    elif text == "SoftmaxCrossEntropy":
        return SoftmaxCrossEntropyLoss()
    elif text == "NegativeLoglikelihood":
        return NegativeLogLikelihoodLoss()
    else:
        raise RuntimeError(f"unknown loss function '{text}'")
